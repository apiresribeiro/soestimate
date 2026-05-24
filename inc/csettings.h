#pragma once

struct info_container;

int read_args_file(const char *args_filename, struct info_container *info);
int read_settings_file(const char *settings_filename, struct info_container *info);
