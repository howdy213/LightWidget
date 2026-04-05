English | [中文](README.md)

# LightWidget

LightWidget is a core plugin of the [WidgetExplorer](https://github.com/howdy213/WidgetExplorer) framework. It provides a concise main window interface for managing other plugins, sending commands, and displaying a system tray icon. Compiling the project generates `LightMain.dll`, which serves as the default `MainWidget`, allowing users to quickly browse loaded plugins, execute commands, and access basic settings.

## Features

- **Plugin List Display**  
  Displays information (name, version, author, path) of all loaded plugins in a table. Double-click an item to copy its content into the command input box.

- **Quick Command Input**  
  Send commands to a specified plugin via the bottom command input box (format: `plugin_name command_arguments`). Press "OK" or Enter to execute.

- **Link List**  
  Reads common commands or links from the `link.txt` file. Click to fill them into the command input box.

- **System Tray**  
  Hides the main window to the system tray when minimized. Double-click the tray icon to show/hide the main window. The tray menu provides "Show" and "Exit" options.

- **Menu Features**  
  - **Plugins**: Create, import, export plugins (placeholders, to be completed)  
  - **Options**: Open settings, restart the application  

## Dependencies

- Qt 6 (Core, GUI, Widgets required)
- [WECore](https://github.com/howdy213/WECore) (WidgetExplorer core library)

## Build

See the [WidgetExplorer](https://github.com/howdy213/WidgetExplorer) project.

## License

This project is licensed under the Apache License 2.0.  
See the [LICENSE](LICENSE) file for details.