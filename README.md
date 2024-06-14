# Attachable Clicker

**Attachable Clicker** is a sophisticated automation tool designed for Windows, allowing users to automate click inputs with precision. It can attach to any window using its Hardware ID (HWID), enabling automation without the need for the window to be in focus; it can even be minimized.

## Key Features

- **Window Attachment:** Securely attaches to windows using their Hardware ID (HWID) for targeted and precise automation.
- **Customizable Click Patterns:** Allows users to record specific click patterns and replay them continuously or for predetermined durations.
- **Discord Integration:** Offers support and anomaly reporting directly via Discord.
- **Anomaly Detection:** Identifies and reports user-defined anomalies, ensuring reliable automation of click patterns.

## Installation

1. Download the latest release from the [Release page](https://github.com/Napalys/Attachable-clicker/releases).
2. Extract the files to your preferred location on your Windows device.
3. Run the executable file to launch the application. Please ensure the software is running with administrator privileges to enable it to send clicks.

## Installation from Source

### Windows

Ensure you have Visual Studio Compiler installed on your Windows machine. If you haven't installed vcpkg yet, follow these steps:

1. Clone the vcpkg repository:
    ```sh
    git clone https://github.com/microsoft/vcpkg.git
    ```

2. Navigate to the vcpkg directory:
    ```sh
    cd vcpkg
    ```

3. Bootstrap vcpkg:
    ```sh
    .\bootstrap-vcpkg.bat
    ```

4. Install dependencies:
    ```sh
    vcpkg install opencv dpp qt5
    ```

To compile the project, follow these steps:

1. Open a command prompt and navigate to the root directory of your project.
2. Run the following command to configure the project:
    ```sh
    cmake -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=path_to_your_vcpkg/scripts/buildsystems/vcpkg.cmake -B build
    ```

3. Once the configuration is complete, build the project with:
    ```sh
    cmake --build build --config Release
    ```

### Linux

To get started, follow these steps:

1. Get the Docker image from Docker Hub: [Attachable Clicker Docker Image](https://hub.docker.com/r/napalys/attachable-clicker-docker/tags)

2. Clone the repository inside the Docker container and build the project:
    ```sh
    mkdir build && cd build && cmake .. && make
    ```


## Getting Started

To begin using the Attachable Clicker, follow these setup steps:

> **Note:** It is essential to run the application as an administrator to ensure full functionality.

### Step 1: Identify the Window
- **Option 1:** Open the Task Manager, locate the Process ID (PID) of the window you wish to automate.
- **Option 2:** Use the "Select Window" feature to click on the window directly. (If you choose this option, skip Step 2.)

### Step 2: Attach to the Window
- Input the window's title into Attachable Clicker.
- Click 'Attach to Window'. A green 'Success' message indicates a successful attachment. If not, verify your inputs.

### Step 3: Configure Clicks
- Click 'Record' to log the desired click patterns you want to automate.

### Step 4: Register Discord Bot (Optional)
- Register a discord bot using the provided bot token and channel ID.
- Add the anomalies you wish to monitor.

### Step 5: Start Automation
- Activate the clicker to begin sending automated clicks to the attached window.

![image](https://github.com/Napalys/Attachable-clicker/assets/11835209/1b2827c2-eb99-4150-8825-957971953884)

