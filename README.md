# LightWidget

LightWidget 是 [WidgetExplorer](https://github.com/howdy213/WidgetExplorer) 框架的一个核心插件，提供了一个简洁的主窗口界面，用于管理其他插件、发送命令以及显示系统托盘图标。编译项目将生成LightMain.dll，并作为默认的 `MainWidget`，让用户能够快速浏览已加载插件、执行命令并访问基本设置。

## 功能特性

- **插件列表展示**  
  以表格形式列出所有已加载插件的信息（名称、版本、作者、路径），并支持双击将内容复制到命令输入框。
- **快捷命令输入**  
  通过底部命令输入框，可以向指定插件发送命令（格式：`插件名 命令参数`），点击“确定”或回车执行。
- **链接列表**  
  从 `link.txt` 文件中读取常用命令或链接，点击即可填充到命令输入框。
- **系统托盘**  
  最小化时隐藏至系统托盘，双击托盘图标显示/隐藏主窗口；托盘菜单提供“显示”和“退出”选项。
- **菜单功能**  
  - **插件**：新建、导入、导出插件（占位，待完成）  
  - **选项**：打开设置、重启应用  

## 依赖项

- Qt 6（需 Core, GUI, Widgets）
- [WECore](https://github.com/howdy213/WECore)（WidgetExplorer 核心库）

## 构建

见 [WidgetExplorer](https://github.com/howdy213/WidgetExplorer) 项目

## 许可证

本项目采用 Apache License 2.0 许可
详情请参见 [LICENSE](LICENSE) 文件
