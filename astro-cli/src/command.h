#ifndef COMMAND_H
#define COMMAND_H

// Handler for the 'data' command
int handle_data_command(int argc, char *argv[]);

// Handler for the 'config' command
int handle_config_command(int argc, char *argv[]);

// Handler for the 'chart' command
int handle_chart_command(int argc, char *argv[]);

// Handler for the 'predict' command
int handle_predict_command(int argc, char *argv[]);

#endif // COMMAND_H
