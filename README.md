# Tennis for Two (C++ & SDL2)

A retro-style tennis game built using C++ and the SDL2 library.

## Prerequisites

* **Visual Studio** (2019 or 2022 recommended) with "Desktop development with C++" installed.
* **SDL2**: The project is configured to look for SDL2 libraries in the relative directories included in this repository.

## How to Build & Run

To ensure all file paths and dependencies resolve correctly, please follow these steps:

1.  **Clone the Repository**
    Do not download as a generic ZIP if possible, as this may alter directory nesting.
    ```bash
    git clone [YOUR_GITHUB_LINK_HERE]
    ```

2.  **Open the Solution**
    Navigate to the root folder and double-click `TennisGame.sln` (or your specific .sln name) to open the project in Visual Studio.

3.  **Compile**
    * Set the build configuration to **Debug** or **Release** (x64 recommended).
    * Press `Ctrl + Shift + B` or go to **Build > Build Solution**.

4.  **Run**
    Press `F5` to start the application.

    *Note: If the game window closes immediately or fails to launch, ensure `SDL2.dll` is located inside the `Tennis` directory.*

## Controls

* **B**: Lob
* **Space**: Serve / Forehand
* **Esc**: Exit

## Project Structure

* `/Tennis`: Contains all `.cpp` and `.h` source files.
* `/lib`: Contains SDL2 library dependencies.
* `/include`: Contains SDL2 include directories.
