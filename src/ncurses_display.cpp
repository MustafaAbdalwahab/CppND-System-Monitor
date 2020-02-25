#include <curses.h>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "format.h"
#include "ncurses_display.h"
#include "system.h"

using std::string;
using std::to_string;

int highlight{0};
int current_pid{0};
// 50 bars uniformly displayed from 0 - 100 %
// 2% is one bar(|)
std::string NCursesDisplay::ProgressBar(float percent) {
  std::string result{"0%"};
  int size{50};
  float bars{percent * size};

  for (int i{0}; i < size; ++i) {
    result += i <= bars ? '|' : ' ';
  }

  string display{to_string(percent * 100).substr(0, 4)};
  if (percent < 0.1 || percent == 1.0)
    display = " " + to_string(percent * 100).substr(0, 3);
  return result + " " + display + "/100%";
}

void NCursesDisplay::DisplaySystem(System& system, WINDOW* window) {
  int row{0};
  mvwprintw(window, ++row, 2, ("OS: " + system.OperatingSystem()).c_str());
  mvwprintw(window, ++row, 2, ("Kernel: " + system.Kernel()).c_str());
  mvwprintw(window, ++row, 2, "CPU: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "");
  wprintw(window, ProgressBar(system.Cpu().Utilization()).c_str());
  wattroff(window, COLOR_PAIR(1));
  mvwprintw(window, ++row, 2, "Memory: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "");
  wprintw(window, ProgressBar(system.MemoryUtilization()).c_str());
  wattroff(window, COLOR_PAIR(1));
  mvwprintw(window, ++row, 2,
            ("Total Processes: " + to_string(system.TotalProcesses())).c_str());
  mvwprintw(
      window, ++row, 2,
      ("Running Processes: " + to_string(system.RunningProcesses())).c_str());
  mvwprintw(window, ++row, 2,
            ("Up Time: " + Format::ElapsedTime(system.UpTime()) + " ").c_str());
  wrefresh(window);
}

void NCursesDisplay::DisplayProcesses(std::vector<Process>& processes,
                                      WINDOW* window, int n) {
  int row{0};
  int const pid_column{2};
  int const user_column{9};
  int const cpu_column{16};
  int const ram_column{26};
  int const time_column{35};
  int const command_column{46};
  wattron(window, COLOR_PAIR(2));
  mvwprintw(window, ++row, pid_column, "PID");
  mvwprintw(window, row, user_column, "USER");
  mvwprintw(window, row, cpu_column, "CPU[%%]");
  mvwprintw(window, row, ram_column, "RAM[MB]");
  mvwprintw(window, row, time_column, "TIME+");
  mvwprintw(window, row, command_column, "COMMAND");
  wattroff(window, COLOR_PAIR(2));
  for (int i = 0; i < n; ++i) {
    if (i == highlight) {
      current_pid = processes[i].Pid();
      wattron(window, A_REVERSE);
    }
    mvwprintw(window, ++row, pid_column, to_string(processes[i].Pid()).c_str());
    mvwprintw(window, row, user_column, processes[i].User().c_str());
    float cpu = processes[i].CpuUtilization() * 100;
    mvwprintw(window, row, cpu_column, to_string(cpu).substr(0, 4).c_str());
    mvwprintw(window, row, ram_column, processes[i].Ram().c_str());
    mvwprintw(window, row, time_column,
              Format::ElapsedTime(processes[i].UpTime()).c_str());
    mvwprintw(window, row, command_column,
              processes[i].Command().substr(0, window->_maxx - 46).c_str());
    if (i == highlight) wattroff(window, A_REVERSE);
  }
}

void NCursesDisplay::DisplayControls(int key[[maybe_unused]], WINDOW* window) {
  int row{0};
  noecho();
  mvwprintw(window, 1, ++row, "s");
  init_pair(9, COLOR_BLACK, COLOR_WHITE);
  wattron(window, COLOR_PAIR(9));
  ++row;
  mvwprintw(window, 1, ++row, "Sort");
  wattroff(window, COLOR_PAIR(9));

  row += 5;

  mvwprintw(window, 1, ++row, "S");
  init_pair(9, COLOR_BLACK, COLOR_WHITE);
  wattron(window, COLOR_PAIR(9));
  ++row;
  mvwprintw(window, 1, ++row, "Search");
  wattroff(window, COLOR_PAIR(9));

  row += 7;

  mvwprintw(window, 1, ++row, "K");
  init_pair(9, COLOR_BLACK, COLOR_WHITE);
  wattron(window, COLOR_PAIR(9));
  ++row;
  mvwprintw(window, 1, ++row, "Kill");
  wattroff(window, COLOR_PAIR(9));

  row += 5;

  mvwprintw(window, 1, ++row, "Q");
  init_pair(9, COLOR_BLACK, COLOR_WHITE);
  wattron(window, COLOR_PAIR(9));
  ++row;
  mvwprintw(window, 1, ++row, "Exit");
  wattroff(window, COLOR_PAIR(9));

  wrefresh(window);
}



void NCursesDisplay::UserInput(int key, int n) {
  switch (key) {
    case KEY_UP:
      highlight--;
      break;
    case KEY_DOWN:
      highlight++;
      break;
    case 'k':
      //TODO: kill the current Process
      //System::kill(current_pid);
      break;
    case 'S':
      //TODO: Search by name
      break;
    case 's':
      //TODO: sort by feature (ram,cpu,pid,command)
      break;
    default:
      break;
  }

  if (highlight < 0) highlight = 0;
  highlight = highlight % n;
}

void NCursesDisplay::Display(System& system, int n) {
  initscr();      // start ncurses
  noecho();       // do not print input values
  cbreak();       // terminate ncurses on ctrl + c
  start_color();  // enable color
  keypad(stdscr, true);
  curs_set(0);  // hide the cursor

  timeout(400);

  int x_max{getmaxx(stdscr)};
  WINDOW* system_window = newwin(9, x_max - 1, 0, 0);
  WINDOW* process_window =
      newwin(3 + n, x_max - 1, system_window->_maxy + 1, 0);
  WINDOW* controls_window =
      newwin(3, x_max - 1, process_window->_maxy * 2 - 2, 0);

  int key;
  while ((key = wgetch(stdscr)) != 113) {
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    box(system_window, 0, 0);
    box(process_window, 0, 0);
    box(controls_window, 0, 0);
    DisplaySystem(system, system_window);
    DisplayProcesses(system.Processes(), process_window, n);
    DisplayControls(key, controls_window);
    wrefresh(system_window);
    wrefresh(process_window);
    wrefresh(controls_window);
    refresh();
    // std::this_thread::sleep_for(std::chrono::seconds(1));

    UserInput(key, n);
  }
  endwin();
}