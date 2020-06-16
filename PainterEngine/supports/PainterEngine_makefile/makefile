target = painterengine.a
core_path = ../core/
kernel_path = ../kernel/
architecture_path = ../architecture/
release_path = ../release/

source_core = $(wildcard $(core_path)*.c)
core_obj = $(patsubst %.c,%.o,$(source_core))

source_kernel = $(wildcard $(kernel_path)*.c)
kernel_obj = $(patsubst %.c,%.o,$(source_kernel))

source_architecture = $(wildcard $(architecture_path)*.c)
architecture_obj = $(patsubst %.c,%.o,$(source_architecture))

$(target):$(core_obj) $(kernel_obj) $(architecture_obj)
	ar -crv $(release_path)$@ $^

%.o:$(core_path)%.c
	gcc -c $^ -o $@

%.o:$(kernel_path)%.c
	gcc -c $^ -o $@

%.o:$(architecture_path)%.c
	gcc -c $^ -o $@

.PHONY:linux_clean
linux_clean:
	-rm $(target) $(build_obj)

.PHONY:win_clean
win_clean:
	-del /s "$(core_path)*.o"
	-del /s "$(kernel_path)*.o"
	-del /s "$(architecture_path)*.o"
	-del "$(release_path)$(target)"