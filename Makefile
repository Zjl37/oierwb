cc = g++ 
flags = -Wall -DUNICODE -lgdiplus
obj = src\main.o src\wbStroke.o src\wbText.o src\wbGraph.o
src = src\main.cpp src\wbStroke.cpp src\wbText.cpp src\wbGraph.cpp

oierwb.exe: $(obj)
	$(cc) $(obj) -o $@ $(flags) -mwindows

%.o: %.cpp
	$(cc) -c $< -o $@ $(flags)

clean:
	del /F $(obj)