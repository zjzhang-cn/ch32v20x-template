# CH32V203 Template Project

基于 CH32V203 RISC-V 微控制器的项目模板，支持 FreeRTOS 和 Docker 容器化开发环境。

## 项目简介

本项目是 CH32V203 系列微控制器的开发模板，提供了完整的构建系统和开发环境。通过 Docker 容器封装了完整的工具链，使得开发环境配置更加简单和一致。

### 主要特性

- ✅ 支持 CH32V203 系列微控制器（CH32V203、CH32V203RB、CH32V208）
- ✅ 集成 FreeRTOS 实时操作系统（可选）
- ✅ Docker 容器化开发环境
- ✅ 完整的 Makefile 构建系统
- ✅ 包含 WCH 官方 EVT 示例代码库

## 硬件支持

- **CH32V203**: CH32V20x_D6 配置
- **CH32V203RB**: CH32V20x_D8 配置
- **CH32V208**: CH32V20x_D8W 配置

## 环境要求

### 使用 Docker（推荐）

- Docker

### 本地开发

- RISC-V GCC 工具链（riscv-wch-elf）
- OpenOCD（用于调试和烧录）

## 快速开始

### 1. 克隆项目

```bash
git clone <your-repo-url>
cd ch32v203-template
git submodule update --init --recursive
```

### 2. 编译项目

使用 Docker 环境（推荐）：

```bash
./run.sh make
```

或直接使用 make（需要本地配置工具链）：

```bash
make
```

### 3. 清理构建文件

```bash
./run.sh make clean
```

### 4. 烧录程序

```bash
./run.sh make flash
```

## 项目结构

```
.
├── Makefile              # 主 Makefile 文件
├── rules.mk              # 通用构建规则
├── run.sh                # Docker 容器运行脚本
├── Build/                # 编译输出目录
│   └── app.lst           # 生成的程序清单文件
└── ch32v20x/             # WCH 官方 EVT 库（子模块）
    ├── EVT/              # 示例代码和库文件
    │   └── EXAM/
    │       ├── FreeRTOS/ # FreeRTOS 相关代码
    │       └── SRC/      # 外设驱动源码
    ├── Datasheet/        # 数据手册
    └── SCHPCB/           # 原理图和 PCB 参考设计
```

## 配置说明

### Makefile 配置项

在 `Makefile` 中可以配置以下选项：

```makefile
# 项目名称
PROJECT = app

# 是否使用 FreeRTOS (y:启用, n:禁用)
USE_FREERTOS = y

# GCC 工具链路径（本地开发时需要）
GCC_TOOCHAIN = /opt/toolchain/riscv-wch-embed/bin/

# OpenOCD 路径（本地开发时需要）
OPENOCD_PATH = /opt/openocd/bin/

# 芯片型号配置
# CH32V203:   CH32V20x_D6
# CH32V203RB: CH32V20x_D8
# CH32V208:   CH32V20x_D8W
LIB_FLAGS = CH32V20x_D6

# 链接脚本
LDSCRIPT = ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/Ld/Link.ld
```

### 添加自定义源文件

在 `Makefile` 中添加你的源文件：

```makefile
# C 源文件目录
CDIRS := your_source_dir \
         another_dir

# 单个 C 源文件
CFILES := your_file.c

# 汇编源文件目录
ADIRS := your_asm_dir

# 单个汇编文件
AFILES := your_startup.S

# 头文件包含路径
INCLUDES := your_include_dir
```

## Docker 环境

本项目使用 `zjzhang/wch:latest` Docker 镜像，其中包含：

- RISC-V GCC 工具链
- OpenOCD
- 其他必要的开发工具

`run.sh` 脚本会自动挂载当前目录并在容器中执行命令。

## 开发工作流

1. **修改代码**: 在 `ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/User` 目录下编辑主程序
2. **编译**: 运行 `./run.sh make`
3. **检查输出**: 查看 `Build/` 目录下的生成文件
4. **烧录**: 运行 `./run.sh make flash`（需要连接硬件）
5. **调试**: 使用 OpenOCD 和 GDB 进行调试

## 常见问题

### 子模块没有正确初始化

```bash
git submodule update --init --recursive
```

### Docker 权限问题

确保当前用户在 `docker` 组中，或使用 `sudo` 运行：

```bash
# 编译并用OpenOCD写入
./run.sh make OPT="-DSDI_PRINT=0" clean all flash
# 使用wlink 通过 SW接口 写入
./run.sh wlink flash ./Build/app.hex
./run.sh wlink sdi-print enable 
# 使用ISP协议通过串口或者USB写入
./run.sh wchisp flash ./Build/app.hex 
#启动openocd服务
./run.sh openocd -f /opt/openocd/bin/wch-riscv.cfg
```

### 编译错误

1. 检查工具链路径配置是否正确
2. 确认 `LIB_FLAGS` 与目标芯片匹配
3. 检查链接脚本路径是否正确

## 相关资源

- [CH32V203 数据手册](ch32v20x/Datasheet/)
- [WCH 官方网站](https://www.wch.cn/)
- [OpenWCH GitHub](https://github.com/openwch)
- [CH32V20x EVT 库](https://github.com/openwch/ch32v20x)
## VSCODE devcontainer
### Dockerfile
```Dockerfile
ARG DOCKER_TAG=latest
FROM zjzhang/wch:${DOCKER_TAG}
ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8
CMD ["/bin/bash", "-c"]
```
### devcontainer.json
```json
{
	"name": "WCH Development",
	"build": {
		"dockerfile": "Dockerfile"
	},
    "privileged": true,
	"customizations": {
		"vscode": {
			"settings": {
				"terminal.integrated.profiles.linux": {
					"bash": {
						"path": "/bin/bash"
					}
				}
				
			}
		},
		"extensions": [
			"ms-vscode.cpptools-extension-pack",
			"ms-vscode.cpptools",
			"ms-vscode.hexeditor",
			"marus25.cortex-debug"
		]
	},
	"runArgs": [
		"--network=host",
	]
}
```
## VSOCDE 
### launch.json
```json
{
    // 使用 IntelliSense 了解相关属性。 
    // 悬停以查看现有属性的描述。
    // 欲了解更多信息，请访问: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "cwd": "${workspaceRoot}",
            "executable": "${workspaceRoot}/Build/app.elf",
            "gdbPath": "riscv-wch-elf-gdb",
            "gdbTarget": "localhost:3333",
            "postLaunchCommands": [
                "monitor reset halt",
                "load"
            ],
            "name": "Debug with OpenOCD",
            "request": "launch",
            "type": "cortex-debug",
            "servertype": "openocd",
            "serverpath": "openocd",
            "configFiles": [
                "wch-riscv.cfg"
            ],
            "searchDir": [],
            "runToEntryPoint": "main",
            "showDevDebugOutput": "raw",
            "device": "CH32V203",
            "toolchainPrefix": "riscv-wch-elf-",
        }
    ]
}
```
## 许可证

本模板项目遵循 MIT 许可证。

CH32V20x EVT 库的许可证请参考其[官方仓库](https://github.com/openwch/ch32v20x)。

## 贡献

欢迎提交 Issue 和 Pull Request！

---

**注意**: 首次使用前请确保已正确初始化 git 子模块。
