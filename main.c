/**
 * Final project - Programmieren I
 * Author: johannes-p
 * Due: 2025-02-23
 */

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

int main() {
  FILE *csv = fopen("spaceship_data_angabe.csv", "r");
  FILE *out = fopen("positions.csv", "w");

  fprintf(out, "x,y\n");

  if (csv != NULL) {
    Vec2D current_position = {0, 0};
    Vec2D current_velocity = {0, 0};
    double current_rotation = 0;
    double max_distance = 0;
    double total_distance = 0;

    double max_temperature;
    double min_temperature;
    int temperature_set = 0;

    double temperature_array[100];
    int num_of_timesteps = 0;
    double variance = 0;
    double average = 0;

    while (feof(csv) == 0) {
      double rotation;
      double acceleration;
      double temperature;
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
    printf("Total distance: %lf", total_distance);
  }
  fclose(csv);
  fclose(out);
}