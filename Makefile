all: miniTRON.exe

%.exe:%.c
	@gcc $< -lpthread -o $@

run: miniTRON.exe
	@./miniTRON.exe

clean:
	@rm *.exe
