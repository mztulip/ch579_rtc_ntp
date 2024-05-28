#/bin/bash
set -e

if [[ $1 = "--ram" ]]; then
    LINKER_SCRIPT="ch579_ram.lds"
    
else
    LINKER_SCRIPT="ch579.lds"
fi

TOOLCHAIN="/home/tulip/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-"
TOOLCHAIN_GCC=$TOOLCHAIN"gcc"
TOOLCHAIN_SIZE=$TOOLCHAIN"size"
TOOLCHAIN_OBJDUMP=$TOOLCHAIN"objdump"
TOOLCHAIN_OBJCOPY=$TOOLCHAIN"objcopy"
echo $TOOLCHAIN_GCC

# -c => Compile or assemble the source files, but do not link. 
# -fdata-sections
#-ffunction-sections => Place each function or data item into its own section in the output file if the target supports arbitrary sections. The name of the function or the name of the data item determines the section’s name in the output file. 
# -mthumb => Select between generating code that executes in ARM and Thumb states
GCC_FLAGS="-c -ffunction-sections -fdata-sections -mthumb -mcpu=cortex-m0 -g -mfloat-abi=soft"
INCLUDES_BASIC="-I StdPeriphDriver/inc/ -I CMSIS/Include/"
INCLUDES_LWIP="-iquote lwip-2.1.2/src/include -iquote lwip-2.1.2/src/include/lwip -iquote lwip-2.1.2/src/arch -iquote lwip-2.1.2/src/ -iquote lwip-2.1.2/src/include/lwip/apps"
INLCUDES="$INCLUDES_BASIC -iquote Net/inc $INCLUDES_LWIP -iquote build"

set -x #echo on

rm build/* -f

file1="$TOOLCHAIN_GCC Main.c $INLCUDES -o build/main.o $GCC_FLAGS"
$file1

file2="$TOOLCHAIN_GCC StdPeriphDriver/CH57x_sys.c $INLCUDES -o build/CH57x_sys.o $GCC_FLAGS"
$file2

file3="$TOOLCHAIN_GCC startup.c $INLCUDES -o build/startup.o $GCC_FLAGS"
$file3

file4="$TOOLCHAIN_GCC StdPeriphDriver/CH57x_clk.c $INLCUDES -o build/CH57x_clk.o $GCC_FLAGS"
$file4

file5="$TOOLCHAIN_GCC StdPeriphDriver/CH57x_gpio.c $INLCUDES -o build/CH57x_gpio.o $GCC_FLAGS"
$file5

file6="$TOOLCHAIN_GCC StdPeriphDriver/CH57x_pwr.c $INLCUDES -o build/CH57x_pwr.o $GCC_FLAGS"
$file6

file7="$TOOLCHAIN_GCC StdPeriphDriver/CH57x_uart1.c $INLCUDES -o build/CH57x_uart1.o $GCC_FLAGS"
$file7

file8="$TOOLCHAIN_GCC StdPeriphDriver/CH57x_int.c $INLCUDES -o build/CH57x_int.o $GCC_FLAGS"
$file8

file9="$TOOLCHAIN_GCC StdPeriphDriver/CH57x_uart0.c $INLCUDES -o build/CH57x_uart0.o $GCC_FLAGS"
$file9

file10="$TOOLCHAIN_GCC StdPeriphDriver/CH57x_uart2.c $INLCUDES -o build/CH57x_uart2.o $GCC_FLAGS"
$file10

file="$TOOLCHAIN_GCC StdPeriphDriver/CH57x_uart3.c $INLCUDES -o build/CH57x_uart3.o $GCC_FLAGS"

$file

file="$TOOLCHAIN_GCC StdPeriphDriver/CH57x_flash.c $INLCUDES -o build/CH57x_flash.o $GCC_FLAGS"

$file

filename=CH57x_timer0
file="$TOOLCHAIN_GCC StdPeriphDriver/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

file="$TOOLCHAIN_GCC stdlib_impl.c $INLCUDES -o build/stdlib_impl.o $GCC_FLAGS"

$file

file="$TOOLCHAIN_GCC Net/src/lwipcomm.c $INLCUDES -o build/lwipcomm.o $GCC_FLAGS"

$file

file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/ipv4/autoip.c $INLCUDES -o build/autoip.o $GCC_FLAGS"

$file

file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/def.c $INLCUDES -o build/def.o $GCC_FLAGS"

$file

file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/init.c $INLCUDES -o build/init.o $GCC_FLAGS"

$file

file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/mem.c $INLCUDES -o build/mem.o $GCC_FLAGS"

$file

file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/memp.c $INLCUDES -o build/memp.o $GCC_FLAGS"

$file

file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/netif.c $INLCUDES -o build/netif.o $GCC_FLAGS"

$file

file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/udp.c $INLCUDES -o build/udp.o $GCC_FLAGS"

$file

file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/tcp.c $INLCUDES -o build/tcp.o $GCC_FLAGS"

$file

file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/timeouts.c $INLCUDES -o build/timeouts.o $GCC_FLAGS"

$file

file="$TOOLCHAIN_GCC lwip-2.1.2/src/netif/ethernet.c $INLCUDES -o build/ethernet.o $GCC_FLAGS"

$file

file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/pbuf.c $INLCUDES -o build/pbuf.o $GCC_FLAGS"

$file

file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/ipv4/ip4.c $INLCUDES -o build/ip4.o $GCC_FLAGS"

$file

filename=ip4_addr
file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/ipv4/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

filename=etharp
file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/ipv4/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

filename=inet_chksum
file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

filename=tcp_in
file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

filename=icmp
file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/ipv4/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

filename=tcp_out
file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

filename=tcp_out
file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

filename=ip
file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

filename=ip4_frag
file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/ipv4/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

filename=dhcp
file="$TOOLCHAIN_GCC lwip-2.1.2/src/core/ipv4/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

filename=eth_mac
file="$TOOLCHAIN_GCC Net/src/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

filename=ethernetif
file="$TOOLCHAIN_GCC Net/src/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

filename=err
file="$TOOLCHAIN_GCC lwip-2.1.2/src/api/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

filename=timer0
file="$TOOLCHAIN_GCC Net/src/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

filename=httpd
file="$TOOLCHAIN_GCC lwip-2.1.2/src/apps/http/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

filename=htmlgen
file="gcc lwip-2.1.2/src/apps/http/makefsdata/makefsdata.c $INLCUDES -o build/$filename"

$file

filename=server_files_packed.h
./build/htmlgen  server_files
mv fsdata.c build/$filename

filename=fs
file="$TOOLCHAIN_GCC lwip-2.1.2/src/apps/http/$filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

filename=rtc
file="$TOOLCHAIN_GCC $filename.c $INLCUDES -o build/$filename.o $GCC_FLAGS"

$file

LINKER_FLAGS="-nostartfiles -Wl,--gc-sections -mcpu=cortex-m0 --specs=nano.specs"
linker="$TOOLCHAIN_GCC build/*.o -T$LINKER_SCRIPT -o output.elf  -Xlinker -Map=output.map $LINKER_FLAGS"
$linker

$TOOLCHAIN_OBJDUMP -S --disassemble -marm output.elf > output.asm
$TOOLCHAIN_OBJCOPY -O ihex output.elf output.hex
$TOOLCHAIN_OBJCOPY -O binary output.elf output.bin

$TOOLCHAIN_SIZE "output.elf"