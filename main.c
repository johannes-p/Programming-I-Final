/**
 * Final project - Programmieren I
 * Author: johannes-p
 * Due: 2025-02-23
 */

#include "latex_report.h"
#include <float.h>
#include <math.h>
#include <stdio.h>

typedef struct Vector2D {
  double x;
  double y;
} Vec2D;

/**
 * @brief Computes the distance between two points using the pythagorean
 * theorem.
 *
 * @param pos1 The starting position as a 2D Vector.
 * @param pos2 The ending position as a 2D Vector.
 * @param dst Pointer to a double where the computed distance will be stored.
 */
void calculate_distance(Vec2D pos1, Vec2D pos2, double *dst) {
  *dst = sqrt(pow(pos2.x - pos1.x, 2) + pow(pos2.y - pos1.y, 2));
}

/**
 * @brief Calculates the variance of an array of doubles.
 *
 * @param values Pointer to an array of doubles.
 * @param size The number of elements in the array (num of timesteps).
 * @return The computed variance of the given array.
 */
double Variance(double values[], int size) {
  if (size <= 1) {
    // Variance cannot be calculated for arrays with 0 or 1 element
    return 0.0;
  }

  double sum = 0.0;
  double sumOfSquares = 0.0;

  for (int i = 0; i < size; i++) {
    sum += values[i];
    sumOfSquares += values[i] * values[i];
  }

  double mean = sum / size;
  double variance = (sumOfSquares / size) - (mean * mean);

  return variance;
}

void rotate(double change_in_rotation, double current_rotation,
            double *new_rotation) {
  *new_rotation = current_rotation + change_in_rotation;
}

/**
 * @brief Calculates the new velocity depending on the spaceships orientation
 *
 * @param current_velocity The current velocity of the spaceship as a 2D Vector.
 * @param acceleration The acceleration to apply to the spaceship.
 * @param rotation The current rotation of the spaceship in radians.
 * @param new_velocity Pointer to a 2D Vector where the new velocity will be
 * saved.
 */
void accelerate(Vec2D current_velocity, double acceleration, double rotation,
                Vec2D *new_velocity) {
  new_velocity->x = current_velocity.x + (cos(rotation) * acceleration);
  new_velocity->y = current_velocity.y + (sin(rotation) * acceleration);
}

/**
 * @brief Calculates the new position of the spaceship.
 *
 * @param current_velocity The current velocity of the spaceship as a 2D Vector.
 * @param current_position The current position of the spaceship as a 2D Vector.
 * @param new_position     Pointer to a 2D Vector where the new position will be
 * saved.
 * @param total_distance   Pointer to a double where the total distance traveled
 * will be saved.
 */
void translate(Vec2D current_velocity, Vec2D current_position,
               Vec2D *new_position, double *total_distance) {
  Vec2D previous_position = current_position;
  double distance;

  new_position->x = current_position.x + current_velocity.x;
  new_position->y = current_position.y + current_velocity.y;

  calculate_distance(previous_position, *new_position, &distance);
  *total_distance += distance;
}

/**
 * @brief Logs positional and temperature data, updating min/max values.
 *
 * @param current_position The current position as a 2D Vector.
 * @param temperature The current temperature reading.
 * @param temperature_set Pointer to an integer flag indicating whether
 * temperature values have been initialized (1 = set, 0 = unset).
 * @param max_distance Pointer to the maximum recorded Euclidean distance of the
 * spaceship to origin, updated if the new distance is greater.
 * @param max_temperature Pointer to the maximum recorded temperature, updated
 * if the new temperature is higher.
 * @param min_temperature Pointer to the minimum recorded temperature, updated
 * if the new temperature is lower.
 */
void log_data(Vec2D current_position, double temperature, int *temperature_set,
              double *max_distance, double *max_temperature,
              double *min_temperature) {
  const Vec2D START_POS = {0, 0};
  double distance;
  calculate_distance(START_POS, current_position, &distance);
  if (distance > *max_distance) {
    *max_distance = distance;
  }

  if (temperature < *min_temperature || !*temperature_set) {
    *min_temperature = temperature;
  } else if (temperature > *max_temperature || !*temperature_set) {
    *max_temperature = temperature;
  }
  *temperature_set = 1;
}

/**
 * @brief Calculates the average of an array of doubles.
 *
 * @param values Pointer to an array of doubles.
 * @param size The number of elements in the array (num of timesteps).
 */
double Average(double values[], int size) {
  double sum = 0;
  for (int i = 0; i < size; i++) {
    sum += values[i];
  }
  return sum / size;
}

/**
 * @brief Saves a path defined by 2D Coordinates in SVG format.
 *
 * @param positions Array of 2D coordinates representing the path.
 * @param offset Offset value to position the path within the visible area.
 */
void save_svg(Vec2D positions[], double offset) {
  FILE *path_line_svg = fopen("line.svg", "w");

  fprintf(path_line_svg,
          "<svg width=\"%f\" height=\"%f\" version=\"1.1\" "
          "xmlns=\"http://www.w3.org/2000/svg\">",
          2 * offset, 2 * offset);

  for (int i = 0; i < 99; i++) {
    double x1, x2, y1, y2 = 0;
    // Invert y-axis coordinates: SVGs positive y points downwards
    // Offset values: to keep the elements within the visible area
    x1 = positions[i].x + offset;
    y1 = -positions[i].y + offset;
    x2 = positions[i + 1].x + offset;
    y2 = -positions[i + 1].y + offset;
    fprintf(path_line_svg,
            "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"%s\" "
            "stroke-width=\"%d\"/>\n",
            x1, y1, x2, y2, "orange", 5);
  }
  fprintf(path_line_svg, "</svg>");
}

/**
 * @brief Calculates a temperature map in form of a multidimensional Array.
 *
 * @param positions Array of 2D coordinates.
 * @param temperatures Array of temperature values at each position.
 * @param total_timesteps Total number of recorded values ("lines in csv").
 * @param matrix_resolution Resolution of the output matrix.
 * @param matrix 2D matrix storing temperature values (for avg calculation).
 */
void temperature_map(Vec2D positions[], double temperatures[],
                     int total_timesteps, int matrix_resolution,
                     double matrix[matrix_resolution][matrix_resolution]) {

  int count[matrix_resolution][matrix_resolution]; // Number of values per cell

  // Initialize arrays
  for (int i = 0; i < matrix_resolution; i++) {
    for (int j = 0; j < matrix_resolution; j++) {
      matrix[i][j] = -DBL_MAX; // Uninitialized marker
      count[i][j] = 0;
    }
  }

  // Find min/max coordinates
  // DBL_MAX is the biggest number a "double" can hold (provided by float.h)
  double min_x = DBL_MAX, min_y = DBL_MAX;
  double max_x = -DBL_MAX, max_y = -DBL_MAX;

  for (int i = 0; i < total_timesteps; i++) {
    if (positions[i].x < min_x)
      min_x = positions[i].x;
    if (positions[i].y < min_y)
      min_y = positions[i].y;
    if (positions[i].x > max_x)
      max_x = positions[i].x;
    if (positions[i].y > max_y)
      max_y = positions[i].y;
  }

  // Calculate the span of one grid cell
  double grid_width = (max_x - min_x) / matrix_resolution;
  double grid_height = (max_y - min_y) / matrix_resolution;

  // Assign points to grid cells
  for (int i = 0; i < total_timesteps; i++) {
    // Determine to which grid cell a measuring point belongs
    int grid_x = (int)((positions[i].x - min_x) / grid_width);
    int grid_y = (int)((positions[i].y - min_y) / grid_height);

    // Ensure within bounds
    if (grid_x >= 0 && grid_x < matrix_resolution && grid_y >= 0 &&
        grid_y < matrix_resolution) {
      if (matrix[grid_x][grid_y] == -DBL_MAX) {
        // First time a value would be added to a grid cell, set it to that
        // temperature value
        matrix[grid_x][grid_y] = temperatures[i];
      } else {
        matrix[grid_x][grid_y] += temperatures[i]; // Sum up temperature
      }
      // Increase "number of added values" - used to calculate the avg
      count[grid_x][grid_y]++;
    }
  }

  // Compute averages
  // The direction of iteration is predetermined by how the trajectory is
  // expected to be oriented
  for (int j = matrix_resolution - 1; j >= 0; j--) {
    // Go through columns from right to left
    for (int i = 0; i < matrix_resolution; i++) {
      // Go through rows top to bottom
      if (count[i][j] > 0) {
        matrix[i][j] /= count[i][j]; // calculate average temperature
      }
      printf(matrix[i][j] > -DBL_MAX ? " ■ " : " ? ");
      // printf(matrix[i][j] > -DBL_MAX ? " %.1lf " : " ? ", matrix[i][j]);
    }
    printf("\n");
  }
}

/**
 * @brief Saves the temperature map in SVG format.
 *
 * @param matrix Multidimensional Array containing the average values calculated
 * by the ´temperature_map´ function.
 * @param resolution Matrix Resolution to traverse the multidimensional array.
 */
void save_temperature_map_svg(double *matrix, int resolution) {
  const char *filename = "temperature_map.svg";
  FILE *file = fopen(filename, "w");
  if (!file) {
    printf("Error: Could not open file %s for writing.\n", filename);
    return;
  }

  int cell_size = 20;

  // calculate required svg width/height
  int width = resolution * cell_size;
  int height = resolution * cell_size;

  fprintf(
      file,
      "<svg width=\"%d\" height=\"%d\" xmlns=\"http://www.w3.org/2000/svg\" "
      "style=\"transform:rotate(-90deg)\">\n",
      width, height);

  double min_temp = DBL_MAX, max_temp = -DBL_MAX;

  // Find min/max temperatures
  for (int i = 0; i < resolution; i++) {
    for (int j = 0; j < resolution; j++) {
      double temp = matrix[i * resolution + j];
      if (temp > -DBL_MAX) { // Ignore "uninitialized" (-DBL_MAX)
        if (temp < min_temp)
          min_temp = temp;
        if (temp > max_temp)
          max_temp = temp;
      }
    }
  }

  // Prevent division by zero (when calculating normalized temperature)
  if (max_temp == min_temp) {
    max_temp += 1.0;
  }

  // Place SVG rectangles
  // A style attribute on the SVG ensures the correct trajectory orientation
  // instead of changing the iteration direction
  for (int i = 0; i < resolution; i++) {
    for (int j = 0; j < resolution; j++) {
      if (matrix[i * resolution + j] == -DBL_MAX) {
        fprintf(file,
                "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" "
                "fill=\"rgb(0,0,0)\" />\n",
                j * cell_size, i * cell_size, cell_size, cell_size);
      } else {
        // Normalize temperature to [0,1] for color intensity
        double norm_temp =
            (matrix[i * resolution + j] - min_temp) / (max_temp - min_temp);
        int red = (int)(255 * norm_temp);
        int blue = (int)(255 * (1 - norm_temp));
        int green = 0;

        fprintf(file,
                "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" "
                "fill=\"rgb(%d,%d,%d)\" />\n",
                j * cell_size, i * cell_size, cell_size, cell_size, red, green,
                blue);
      }
    }
  }

  fprintf(file, "</svg>\n");
  fclose(file);
  printf("SVG file saved as %s\n", filename);
}

int main() {
  FILE *csv = fopen("spaceship_data_angabe.csv", "r");
  FILE *out = fopen("positions.csv", "w");
  Vec2D positions[100];
  double temperatures[100];

  int matrix_resolution = 25;
  double temperature_matrix[matrix_resolution][matrix_resolution];

  double max_distance = 0;
  double total_distance = 0;

  double max_temperature;
  double min_temperature;
  int temperature_set = 0;

  double variance = 0;
  double average = 0;

  fprintf(out, "x,y,rotation\n");

  if (csv != NULL) {
    Vec2D current_position = {0, 0};
    Vec2D current_velocity = {0, 0};
    double current_rotation = 0;

    double temperature_array[100];
    int num_of_timesteps = 0;

    while (feof(csv) == 0) {
      positions[num_of_timesteps] = current_position;

      double rotation;
      double acceleration;
      double temperature;

      temperatures[num_of_timesteps] = temperature;

      fscanf(csv, "%lf,%lf,%lf\n", &acceleration, &rotation, &temperature);

      rotate(rotation, current_rotation, &current_rotation);
      accelerate(current_velocity, acceleration, current_rotation,
                 &current_velocity);
      translate(current_velocity, current_position, &current_position,
                &total_distance);

      log_data(current_position, temperature, &temperature_set, &max_distance,
               &max_temperature, &min_temperature);

      fprintf(out, "%.15lf,%.15lf,%.15lf\n", current_position.x,
              current_position.y, current_rotation);
      temperature_array[num_of_timesteps] = temperature;
      num_of_timesteps++;
    }
    variance = Variance(temperature_array, num_of_timesteps);
    average = Average(temperature_array, num_of_timesteps);

    printf("Max. Temperature: %lf\nMin. Temperature: %lf\n", max_temperature,
           min_temperature);
    printf("Temperature avg: %lf\n", average);
    printf("Temperature variance: %lf\n\n", variance);
    printf("Max. Euclidean distance to start: %lf\n", max_distance);
    printf("Total distance: %lf\n", total_distance);

    save_svg(positions, max_distance);
    temperature_map(positions, temperatures, 100, matrix_resolution,
                    temperature_matrix);
    save_temperature_map_svg(*temperature_matrix, matrix_resolution);
  }
  fclose(csv);
  fclose(out);
  generate_latex_report("report.tex", matrix_resolution, total_distance,
                        max_distance, max_temperature, min_temperature, average,
                        variance);
}