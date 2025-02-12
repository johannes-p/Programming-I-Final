#include "latex_report.h"
#include <stdio.h>

void generate_pgfplots_plot(const char *spaceship_data_filename,
                            FILE *latex_report) {
  FILE *spaceship_data_file = fopen(spaceship_data_filename, "r");
  if (!spaceship_data_file) {
    perror("Error opening file");
    return;
  }

  fprintf(latex_report, "\\begin{center}");
  fprintf(latex_report, "\\begin{tikzpicture}\n");
  fprintf(latex_report, "\t\\begin{axis}[\n");
  fprintf(latex_report, "\t\txlabel={Seconds},\n");
  fprintf(latex_report, "\t\tylabel={Temperature},\n");
  fprintf(latex_report, "\t\tgrid=major\n");
  fprintf(latex_report, "\t]\n");
  fprintf(latex_report, "\t\t\\addplot[smooth, thick, blue] coordinates {");

  int counter = 1;
  double temperature;

  // The * in %*lf supresses assignment, the values are read but not assigned to
  // any memory location, as acceleration and relative rotation aren't needed
  // right now (for the temperature plot)
  while (fscanf(spaceship_data_file, "%*lf,%*lf,%lf", &temperature) == 1) {
    fprintf(latex_report, "(%d,%.2f) ", counter, temperature);
    counter++;
  }

  fprintf(latex_report, "};\n");
  fprintf(latex_report, "\t\\end{axis}\n");
  fprintf(latex_report, "\\end{tikzpicture}");
  fprintf(latex_report, "\\end{center}");
}

void generate_latex_report(const char *filename,
                           const char *spaceship_data_filename, int resolution,
                           double total_distance, double farthest_from_start,
                           double max_temp, double min_temp, double avg_temp,
                           double var_temp) {
  FILE *file = fopen(filename, "w");
  if (!file) {
    perror("Error opening file");
    return;
  }

  // Start LaTeX document
  fprintf(file, "\\documentclass[12pt]{article}\n\n");
  fprintf(file, "\\usepackage[utf8]{inputenc}\n");
  fprintf(file, "\\usepackage{latexsym,amsmath}\n");
  fprintf(file, "\\usepackage{longtable}\n");
  fprintf(file, "\\usepackage{svg}\n");
  fprintf(file, "\\usepackage{pgfplots}\n\n");
  fprintf(file, "\\pgfplotsset{compat=1.18}\n\n");
  fprintf(file, "\\setlength{\\parindent}{0in}\n");
  fprintf(file, "\\setlength{\\oddsidemargin}{0in}\n");
  fprintf(file, "\\setlength{\\textwidth}{6.5in}\n");
  fprintf(file, "\\setlength{\\textheight}{8.8in}\n");
  fprintf(file, "\\setlength{\\topmargin}{0in}\n");
  fprintf(file, "\\setlength{\\headheight}{18pt}\n");

  fprintf(file, "\\title{Flight report}\n");
  fprintf(file, "\\author{Navigator Nelly McDetour}\n");
  fprintf(file, "\\begin{document}\n");
  fprintf(file, "\\maketitle\n");
  fprintf(file, "\\begin{figure}[htp]\n");
  fprintf(file, "\\centering\n");
  fprintf(file, "\\includesvg{line.svg}\n");
  fprintf(file, "\\caption{Trajectory}\n");
  fprintf(file, "\\end{figure}\n\n");

  fprintf(file, "\\newpage\n");
  fprintf(file, "\\subsection*{Position \\& Orientation}\n");
  fprintf(file, "\\begin{longtable}{|c|c|c|c|}\n");
  fprintf(file,
          "\\caption{Position and Rotation} \\label{tab:coordinates}\n\n");
  fprintf(file, "\\\\\\hline\n");
  fprintf(file, "\\textbf{Time} & \\textbf{X-COORD} & \\textbf{Y-COORD} & "
                "\\textbf{Rotation (radians)} \\\\  \n");
  fprintf(file, "\\hline\n");
  fprintf(file, "\\endfirsthead\n");
  fprintf(file, "\n");
  fprintf(file, "\\hline\n");
  fprintf(file, "\\textbf{Time} & \\textbf{X-COORD} & \\textbf{Y-COORD} & "
                "\\textbf{Rotation (radians)} \\\\  \n");
  fprintf(file, "\\hline\n");
  fprintf(file, "\\endhead");

  FILE *orientation_file = fopen("positions.csv", "r");
  if (!orientation_file) {
    perror("Error opening positions.csv - unable to generate LaTeX report");
    fclose(file);
    return;
  }

  // Discard the column names by saving them to a buffer and in doing so
  // incrementing the position offset
  char header[100]; // Buffer to hold the first line (column names)
  fgets(header, sizeof(header),
        orientation_file); // Read (and discard) the header line

  int counter = 1;
  double x, y, rotation;
  // Using feof(*FILE) == 0 is bad practice!
  while (fscanf(orientation_file, " %lf,%lf,%lf", &x, &y, &rotation) == 3) {
    fprintf(file, "%d & %.6f & %.6f & %.6f \\\\ \\hline\n", counter, x, y,
            rotation);
    counter++;
  }
  fclose(orientation_file);
  fprintf(file, "\\hline\n");
  fprintf(file, "\\end{longtable}");

  fprintf(file, "\\newpage\n");
  fprintf(file, "\\subsection*{Temperature readings}\n");
  fprintf(file, "\\begin{figure}[htp]\n");
  fprintf(file, "\t\\centering\n");
  fprintf(file, "    "
                "\\includesvg[width=0.5\\linewidth,angle=90,origin=c]{"
                "temperature_map.svg}\n");
  fprintf(file, "\t\\caption{Temperature heatmap %ix%i}\n", resolution,
          resolution);
  fprintf(file, "\\end{figure}\n");
  fprintf(file, "\\begin{align*}\n");
  fprintf(file, "\t\\text{Temperature Average:} & \\quad %.2lf \\\\\n",
          avg_temp);
  fprintf(file, "\t\\text{Temperature Variance:} & \\quad %.2lf \\\\\n",
          var_temp);
  fprintf(file, "\\end{align*}\n");
  generate_pgfplots_plot(spaceship_data_filename, file);

  fprintf(file, "\\subsection*{Mission Summary}\n");
  fprintf(file, "\\begin{align*}\n");
  fprintf(file, "\t\\text{Total distance covered:} & \\quad %.3lf \\\\\n",
          total_distance);
  fprintf(file,
          "\t\\text{Farthest recorded distance from start:} & \\quad %.3lf "
          "\\\\\n",
          farthest_from_start);
  fprintf(file, "\t\\text{Peak temperature recorded:} & \\quad %.2lf \\\\\n",
          max_temp);
  fprintf(file, "\t\\text{Minimum temperature recorded:} & \\quad %.2lf\n",
          min_temp);

  fprintf(file, "\\end{align*}\n");
  fprintf(file, "\\end{document}");
  fclose(file);
}
