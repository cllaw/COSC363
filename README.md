# COSC363

Set Build Commands

1. Compile: g++ -Wall -c "%f"
2. Build: g++ -Wall -o "%e" "%f" -lGL -lGLU -lglut
3. Lint: cppcheck --language=c++ --enable=warning,style --template=gcc "%f"
