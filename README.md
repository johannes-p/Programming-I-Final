# 🚀 Spaceship Flight Analysis Project

## 📖 Introduction
This project is part of the **"Programmieren I"** course and is centered around analyzing and visualizing the flight path of a spaceship based on data from a provided `.csv` file. The project involves processing time-step data, calculating various metrics, and generating visualizations to understand the spaceship's movement and the measured temperatures.

### Key Features:
- **Input:** A `.csv` file containing acceleration, rotation, and temperature data over 100 time steps.
- **Output:** Calculated metrics, visualizations, and optional reports in various formats.

---

## ▶️ Usage 

### **Compilation**  
To compile the program, use the following command:  

```sh
gcc main.c latex_report.c -o [output_filename]
```

Replace `[output_filename]` with your desired executable name.  

### **LaTeX Compilation**  
To compile the LaTeX report, use:  

```sh
pdflatex --shell-escape report.tex
```

The `--shell-escape` flag is required for SVG conversion. Ensure that **Inkscape** is installed and available in your system’s PATH for proper conversion.  

---

## ✅ Minimum Grading Criteria Checklist
To secure a positive grade, the following tasks are implemented:

- [x] **Read Data**: Parse the `.csv` file to extract spaceship instructions and measurements.
- [x] **Max Speed**: Calculate the maximum velocity achieved during the flight.
- [x] **Output CSV**: Generate a `.csv` file containing the spaceship's position and orientation at each time step.
- [x] **Max Distance**: Determine the maximum distance from the starting position during the flight.
- [x] **Total Path Length**: Calculate the total distance traveled.
- [x] **Temperature Stats**: Compute the following temperature metrics:
  - Maximum
  - Minimum
  - Average
  - Variance

---

## ✨ Additional Work (Optional Extensions)
To achieve a better grade, the following extensions are implemented:

- [x] **SVG Trajectory**: Save the flight path as an `.svg` file.
- [x] **SVG Heatmap**: Save a heatmap based on averaged temperatures as an `.svg` file.
- [x] **LaTeX Report**: Generate a report containing all calculated values and visuals.
- [X] **Temperature Plot**: Generate a temperature plot using Pgfplots.
- [X] **Console Menu**: Allow the user to configure the program behavior via checkboxes.
  
🚀 Mission complete. Thanks for being part of the journey! 🌌

---

## 📊 Expected Output
- **Maximum Speed**: `5.273075844681093 m/s`
- **Maximum Distance**: `169.69419409810632 m`
- **Average Temperature**: `23.26417185075789 °C`

### Visualizations:
1. **Flight Path Visualization**:
   
   ![Flight Path](path_line.svg)

---
