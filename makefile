all:main

main:
	g++ main.cpp glad/glad.c -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -o tp2Exec
clean:
	rm tp2Exec
