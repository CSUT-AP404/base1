CXX = g++
CXXFLAGS = -std=c++17 -Wall

ADMIN_TARGET = src/admin
USER_TARGET = src/user

all: $(ADMIN_TARGET) $(USER_TARGET)

$(ADMIN_TARGET): src/admin.cpp
	$(CXX) $(CXXFLAGS) -o $(ADMIN_TARGET) src/admin.cpp

$(USER_TARGET): src/user.cpp
	$(CXX) $(CXXFLAGS) -o $(USER_TARGET) src/user.cpp

run-admin: $(ADMIN_TARGET)
	./$(ADMIN_TARGET)

run-user: $(USER_TARGET)
	./$(USER_TARGET)

clean:
	rm -f $(ADMIN_TARGET) $(USER_TARGET)
