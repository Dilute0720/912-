> 想要学懂处理器这一章, 最好的办法莫过于自己使用硬件编程语言手写一个简单的处理器了. 本文使用Verilog语言采用模块化的思想, 以最自然的需求导向方式进行MIPS指令集处理器的实现. 模块的设计是简单的, 难点在于数据通路的设计, 以及多数据通路如何复用, 如何满足不同类型指令对相同硬件的需求. 多路选择器MUX和控制信号生成器Controller将会是建立数据通路的核心.

## 大纲: 处理器基本概念

组织部: PC/控制器+mux
执行部: ALU
基层:    寄存器堆/存储器
架构: 此处以MIPS指令集架构为主.

时钟方法: 规定信号读写的时间非常重要, 如果一个信号同时被读写, 读出的信号可能是写入之前/之后的值. 为了避免这种不确定性, 我们采用边沿触发的时钟方法, 只允许在时钟跳变的边沿时改变.

## 建立数据通路

设计数据通路是针对具体的MIPS指令而言的, 然后再考虑同时满足各种指令需要设计怎样的控制信号和多路选择器.

处理器需要指令来告知它需要做什么, 那么首先我们就需要将指令从存储单元中取出, 而为了准备执行下一条指令, 每一轮指令执行完毕后也需要对PC+4.

### R型指令: 寄存器堆与ALU的实现
对R型指令(add, sub, AND, OR, slt), 需要在寄存器堆中读取两个寄存器, 对他们的值进行ALU操作, 再写回结果至寄存器堆中. 因而**寄存器堆**需要3个寄存器地址输入, 一个数据写入输入, 一个写使能控制信号

```verilog
module register_file(
    input wire clk,          // 时钟信号
    input wire we,           // 写使能信号
    input wire [4:0] ra,     // 读地址1
    input wire [4:0] rb,     // 读地址2
    input wire [4:0] wa,     // 写地址
    input wire [31:0] wd,    // 写数据
    output reg [31:0] outa,  // 寄存器A的输出
    output reg [31:0] outb   // 寄存器B的输出
);

    // 定义寄存器堆，这里以32个32位寄存器为例
    reg [31:0] registers[31:0];

    // 在时钟上升沿时进行寄存器的读写操作
    always @(posedge clk) begin
        if (we) begin
            // 写操作
            registers[wa] <= wd;
        end
        // 读操作在组合逻辑中完成，不需要在时钟边沿触发
    end

    // 组合逻辑，根据读地址输出寄存器的值
    assign outa = registers[ra];
    assign outb = registers[rb];

endmodule
```

**ALU**需要对输入的两个32位数据进行计算, 输出到一个32位输出, 一个1位输出指示计算结果是否为0, 并接受一个4位的控制信号决定进行何种运算.

```verilog
module alu(
    input wire [31:0] a,      // 第一个操作数
    input wire [31:0] b,      // 第二个操作数
    input wire [3:0] control, // 4位控制信号
    output reg [31:0] result, // 计算结果
    output reg zero           // 结果是否为0的标志
);

    // 根据控制信号执行不同的运算
    always @(*) begin
        case (control)
            4'b0000: begin  // ADD
                result = a + b;
                zero = (result == 32'b0);
            end
            4'b0001: begin  // SUB
                result = a - b;
                zero = (result == 32'b0);
            end
            // 这里可以继续添加其他ALU操作，例如AND, OR, SLT等
            // 例如：
            // 4'b0100: begin  // SET_LT
            //     result = (a < b) ? 32'b1 : 32'b0;
            //     zero = (result == 32'b0);
            // end
            // ...
            default: begin
                result = 32'bx; // 如果控制信号无效，结果为未知
                zero = 1'bx;    // 零标志也为未知
            end
        endcase
    end

endmodule
```

### 存取指令: 符号扩展与存储器

MIPS中的lw和sw指令接受一个寄存器地址`$1`和一个16位的地址偏移量`offset($2)`, 在这类指令中, 通过将基址寄存器`$2`中的地址和指令中的16位带符号偏移地址相加, 得到存储器地址. 所以我们需要一个单元将16位的偏移地址**符号扩展**为32位.
```verilog
module sign_extend(
    input wire [15:0] in,  // 16位输入
    output reg [31:0] out  // 32位输出
);

    // 符号扩展逻辑
    always @(*) begin
        if (in[15]) begin  // 如果最高位是1，表示负数
            out = 32'hFFFF0000 | in;  // 扩展为32位的负数
        end else begin
            out = in;  // 如果最高位是0，表示正数，直接扩展
        end
    end

endmodule
```

对于给定的地址, 应该能从**存储器**中读取地址中的数据, 这要求存储器有32位的地址输入,MemRead/MemWrite使能信号, 以及32位的写入端口.
```verilog
module memory(
    input wire clk,          // 时钟信号
    input wire MemRead,      // 存储器读使能信号
    input wire MemWrite,     // 存储器写使能信号
    input wire [31:0] addr,  // 32位地址输入
    input wire [31:0] data_in, // 32位数据写入端口
    output reg [31:0] data_out // 32位数据输出端口
);

    // 定义一个简单的存储器数组，这里以1024个32位宽的存储单元为例(也就是10位寻址)
    reg [31:0] mem_array[1023:0];

    // 在时钟上升沿时进行存储器的读写操作
    always @(posedge clk) begin
        if (MemWrite) begin
            // 写操作
            mem_array[addr[10:0]] <= data_in;
        end
        if (MemRead) begin
            // 读操作
            data_out <= mem_array[addr[10:0]];
        end
    end

endmodule
```

### 端口冲突: 数据通路使用mux进行复用

R型指令需要使用ALU, 其输入来自两个寄存器, 而存储指令的地址也是由ALU进行加法运算得出, 其输入一个来自基址寄存器一个来自指令中的立即数, 这样来看ALU是必然有一个端口又能接收寄存器输入, 又能接收指令中立即数的输入了, 如何实现呢? 使用**多路选择器**即可.
```verilog
module mux4(//4选1多路选择器
    input wire [31:0] in0,  // 第一个输入
    input wire [31:0] in1,  // 第二个输入
    input wire [31:0] in2,  // 第三个输入
    input wire [31:0] in3,  // 第四个输入
    input wire [1:0] sel,   // 2位选择信号
    output reg [31:0] out    // 输出
);

    // 根据选择信号选择输出
    always @(*) begin
        case (sel)
            2'b00: out = in0;
            2'b01: out = in1;
            2'b10: out = in2;
            2'b11: out = in3;
            default: out = 32'b0; // 默认情况
        endcase
    end

endmodule
```

当我们把存取指令纳入视野中时, 我们发现**寄存器堆**的写数据端口可能接受ALU的直接输出也可能接受存储器的输出, 具体则需要根据当前指令类型由控制模块生成控制信号管理多路选择器进行数据通路选择. 这种只有两个输入的多路选择器可以集成到模块内部, 正如我们下面的PC实现一样.
### 寻找下一步: 集成的PC实现

PC在常规的情况下每执行一个指令就应该后移指令长度(32位)也就是4Bytes, 常常表示为PC=PC+4; 但当出现分支指令/跳转指令时, PC应当接受ALU计算输出的结果. 我们并不能预知什么时候ALU输出的值是供给PC使用的, 但我们知道当控制器中指令为分支/跳转指令时, PC应当接受ALU的输出. 这样我们只需要让PC模块始终接受ALU的输出并且将该输出和加法器PC+4的输出使用一个MUX中继, 由控制器生成跳转使能信号即可.
```verilog
module PC(
    input wire clk,                 // 时钟信号
    input wire reset,               // 复位信号
    input wire branch,              // 分支使能信号
    input wire [31:0] alu_out,     // ALU输出
    input wire [31:0] next_pc,     // 来自加法器的PC+4的输出
    output reg [31:0] pc           // 程序计数器
);

    // 定义加法器模块实例
    wire [31:0] incremented_pc;
    adder adder_inst(
        .a(pc),
        .b(32'd32),  // pc+=32位地址
        .sum(incremented_pc)
    );

    // 多路选择器，选择PC的来源
    always @(posedge clk or posedge reset) begin
        if (reset) begin
            pc <= 32'h0000_0000;  // 复位时PC重置为起始地址
        end else if (branch) begin
            pc <= alu_out;         // 分支时使用ALU输出
        end else begin
            pc <= incremented_pc;  // 常规指令执行时PC+4
        end
    end

endmodule
```
注意, 这里的branch分支使能信号时分支指令(由控制单元确定)和ALU的zero输出(相等则分支)的与信号(branch if equal,本质是ALU对两个数执行减法运算的结果). 否则不论是否满足分支条件都会直接跳转至分支地址了.
### 简要尝试: 简易MIPS子集的控制信号实现

要根据当前执行的指令类型生成相应的控制信号，以驱动ALU、存储器、寄存器堆等组件, 我们需要下列信号.

1. **ALU控制信号** (`alu_control`): 4位，用于指示ALU执行的操作类型。
2. **寄存器写使能** (`reg_write`): 1位，指示是否写入寄存器。
3. **ALU第二个操作数选择** (`alu_src`): 1位，指示ALU的第二个操作数是来自寄存器还是立即数。
4. **存储器读使能** (`MemRead`): 1位，指示是否从存储器读取数据。
5. **存储器写使能** (`MemWrite`): 1位，指示是否向存储器写入数据。
6. **PC写使能** (`pc_write`): 1位，指示是否写入程序计数器。

```verilog
module controller(
    input wire [31:0] instruction, // 当前执行的指令
    output reg [3:0] alu_control, // ALU控制信号
    output reg reg_write,         // 寄存器写使能
    output reg alu_src,           // ALU第二个操作数选择
    output reg MemRead,           // 存储器读使能
    output reg MemWrite,          // 存储器写使能
    output reg pc_write           // PC写使能
);

    // 指令类型解码-此处仅为例子,具体指令请具体分析.
    always @(*) begin
        case (instruction[31:26])
            6'b000000: begin // R型指令
                alu_control = 4'b0000; // ADD
                reg_write = 1'b1;
                alu_src = 1'b0;
                MemRead = 1'b0;
                MemWrite = 1'b0;
                pc_write = 1'b0;
            end
            6'b100011: begin // lw指令
                alu_control = 4'b0001; // SUB (基地址 + 偏移量)
                reg_write = 1'b1;
                alu_src = 1'b1;
                MemRead = 1'b1;
                MemWrite = 1'b0;
                pc_write = 1'b0;
            end
            6'b101011: begin // sw指令
                alu_control = 4'b0001; // SUB (基地址 + 偏移量)
                reg_write = 1'b0;
                alu_src = 1'b1;
                MemRead = 1'b0;
                MemWrite = 1'b1;
                pc_write = 1'b0;
            end
            6'b000010: begin // j指令
                alu_control = 4'b0000; // NOP (无操作)
                reg_write = 1'b0;
                alu_src = 1'b0;
                MemRead = 1'b0;
                MemWrite = 1'b0;
                pc_write = 1'b1;
            end
            default: begin
                alu_control = 4'b0000; // 默认为NOP
                reg_write = 1'b0;
                alu_src = 1'b0;
                MemRead = 1'b0;
                MemWrite = 1'b0;
                pc_write = 1'b0;
            end
        endcase
    end

endmodule
```

### 指令扩展: 单周期MIPS处理器的实现

我们最后实现的数据通路如下:

![[MIPS单周期处理器简要数据通路.png]]

以上内容已经包含了MIPS核心指令集中绝大多数指令的单周期实现, 接下来我们要在此基础上加入跳转指令.

跳转指令类似于分支指令, 但是它用于计算目标PC的方式不同, 目标地址的最低两位恒为00, 接着是j型指令中低26位的立即数, 高四位来自跳转指令的PC+4(也就是说跳转的范围为当前PC地址+28位偏移量), 这三部分的值在电路中都已经实现了, 但我们需要调整给PC的地址, 在下图右上角添加一个新的MUX用于选择分支/跳转/PC+4.

![[扩展跳转指令的数据通路.png]]

## 后记

至此, 我们的单周期MIPS处理器就大功告成了. 但是我们仍不满意, 每一个时钟周期都只能执行一条指令, 我们不得不把时钟周期延长至满足最坏情况指令执行时间, 这违背了计算机加速大概率事件这一设计原则. 想要提高处理器的效率, 流水线技术在沿用了单周期类似的数据通路的情况下, 重叠多条指令的执行解决了这一问题.

下一篇: [[多周期MIPS处理器]]
