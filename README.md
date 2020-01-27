## libatshell

> 通用终端CLI 命令行工具，参照redis-cli 实现，命令支持可配置化。



### Install

```shell
# 安装依赖库
yum install -y readline-devel

# 编译库
wget https://github.com/spkettas/libshell/archive/master.zip
unzip master.zip
cd master && make

# 编译demo
cd test && make 
```



### 配置文件

config/cmd.xml 

```xml
<?xml version="1.0" encoding="UTF-8"?>
<root>
    <!-- 
	external 0内置命令 1调用外部程序
	arity    参数个数，当为负时参数至少为N个，为正时限制参数个数
    -->
    <Module name="myserv" external="0">
        <cmd id="20001" name="show" arity="-2">
            <subcmd name="mirror" arity="2"/>
            <subcmd name="log" arity="4"/>
        </cmd>

        <cmd id="20002" name="set" arity="-3">
            <subcmd name="mirror" arity="3"/>

            <subcmd name="aaa" arity="-3">
                <property value="server" arity="3"/>
                <property value="interface" arity="4"/>
            </subcmd>
        </cmd>
    </Module>

    <!-- tester -->
    <Module name="tester" external="1" />
</root>
```



### 演示

- 启动服务端

```shell
cd test && ./myserv
```



- 启动shell

![shell](https://github.com/spkettas/libshell/blob/master/screenshot/shell.png "")



### 待完善:smile:

- [ ] 配置文件目前只支持4个以内参数
- [ ] 不支持命令自动补全