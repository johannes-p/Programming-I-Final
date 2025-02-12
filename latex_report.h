#ifndef LATEX_REPORT_H
#define LATEX_REPORT_H

void generate_latex_report(const char *filename,
                           const char *spaceship_data_filename, int resolution,
                           double total_distance, double farthest_from_start,
                           double max_temp, double min_temp, double avg_temp,
                           double var_temp);

#endif // LATEX_REPORT_H