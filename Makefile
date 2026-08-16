CXX = g++
CXXFLAGS = -std=c++17 -Wall

ADMIN_TARGET = src/admin
USER_TARGET = src/user
ADMIN_SERVER = src/admin-server
USER_SERVER = src/user-server

all: $(ADMIN_TARGET) $(USER_TARGET) $(ADMIN_SERVER) $(USER_SERVER)

$(ADMIN_TARGET): src/admin.cpp
	$(CXX) $(CXXFLAGS) -o $(ADMIN_TARGET) src/admin.cpp

$(USER_TARGET): src/user.cpp
	$(CXX) $(CXXFLAGS) -o $(USER_TARGET) src/user.cpp

$(ADMIN_SERVER): src/admin-server.cpp
	$(CXX) $(CXXFLAGS) -o $(ADMIN_SERVER) src/admin-server.cpp

$(USER_SERVER): src/user-server.cpp
	$(CXX) $(CXXFLAGS) -o $(USER_SERVER) src/user-server.cpp

run-admin: $(ADMIN_TARGET)
	./$(ADMIN_TARGET)

run-user: $(USER_TARGET)
	./$(USER_TARGET)

run-server-admin: $(ADMIN_SERVER)
	./$(ADMIN_SERVER)

run-server-user: $(USER_SERVER)
	./$(USER_SERVER)

clean:
	rm -f $(ADMIN_TARGET) $(USER_TARGET) $(ADMIN_SERVER) $(USER_SERVER)
