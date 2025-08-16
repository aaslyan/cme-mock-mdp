# Simple Makefile for quick compilation testing
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread
INCLUDES = -I./include

# Library source files
LIB_SRCS = src/network/udp_publisher.cpp \
           src/network/feed_publisher.cpp \
           src/network/udp_receiver.cpp \
           src/utils/logger.cpp \
           src/core/order_book.cpp \
           src/core/order_book_manager.cpp \
           src/core/market_data_generator.cpp \
           src/core/reference_data.cpp \
           src/messages/sbe_encoder.cpp \
           src/messages/sbe_decoder.cpp \
           src/messages/cme_sbe_encoder.cpp \
           src/messages/message_factory.cpp \
           src/config/configuration.cpp \
           src/scenarios/market_scenario.cpp

# Test programs
TEST_UDP_SRCS = test/test_udp_publisher.cpp
RECEIVER_SRCS = test/udp_receiver.cpp
TEST_BOOK_SRCS = test/test_order_book.cpp
TEST_MDP_SRCS = test/test_mdp_encoding.cpp
TEST_CLIENT_SRCS = test/cme_test_client.cpp
DEBUG_CLIENT_SRCS = test/debug_client.cpp
DEBUG_INC_SRCS = test/debug_incremental.cpp
TEST_REF_DATA_SRCS = test/test_reference_data.cpp
TEST_SCENARIOS_SRCS = test/test_scenarios.cpp
LIST_INSTRUMENTS_SRCS = test/list_instruments.cpp
TEST_SBE_SRCS = test/test_sbe_generated.cpp

# Object files
LIB_OBJS = $(LIB_SRCS:.cpp=.o)
TEST_UDP_OBJS = $(TEST_UDP_SRCS:.cpp=.o)
RECEIVER_OBJS = $(RECEIVER_SRCS:.cpp=.o)
TEST_BOOK_OBJS = $(TEST_BOOK_SRCS:.cpp=.o)
TEST_MDP_OBJS = $(TEST_MDP_SRCS:.cpp=.o)
TEST_CLIENT_OBJS = $(TEST_CLIENT_SRCS:.cpp=.o)
DEBUG_CLIENT_OBJS = $(DEBUG_CLIENT_SRCS:.cpp=.o)
DEBUG_INC_OBJS = $(DEBUG_INC_SRCS:.cpp=.o)
TEST_REF_DATA_OBJS = $(TEST_REF_DATA_SRCS:.cpp=.o)
TEST_SCENARIOS_OBJS = $(TEST_SCENARIOS_SRCS:.cpp=.o)
LIST_INSTRUMENTS_OBJS = $(LIST_INSTRUMENTS_SRCS:.cpp=.o)
TEST_SBE_OBJS = $(TEST_SBE_SRCS:.cpp=.o)

# Targets
TARGETS = cme_mock_server cme_test_client debug_client debug_incremental test_udp_publisher udp_receiver test_order_book test_mdp_encoding test_reference_data test_scenarios list_instruments test_sbe_generated

all: $(TARGETS)

cme_mock_server: src/main.o $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

cme_test_client: $(TEST_CLIENT_OBJS) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

debug_client: $(DEBUG_CLIENT_OBJS) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

debug_incremental: $(DEBUG_INC_OBJS) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

test_udp_publisher: $(TEST_UDP_OBJS) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

udp_receiver: $(RECEIVER_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

test_order_book: $(TEST_BOOK_OBJS) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

test_mdp_encoding: $(TEST_MDP_OBJS) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

test_reference_data: $(TEST_REF_DATA_OBJS) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

test_scenarios: $(TEST_SCENARIOS_OBJS) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

list_instruments: $(LIST_INSTRUMENTS_OBJS) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

test_sbe_generated: $(TEST_SBE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(LIB_OBJS) $(TEST_UDP_OBJS) $(RECEIVER_OBJS) $(TEST_BOOK_OBJS) $(TEST_MDP_OBJS) $(TEST_CLIENT_OBJS) $(DEBUG_CLIENT_OBJS) $(DEBUG_INC_OBJS) $(TEST_REF_DATA_OBJS) $(TEST_SCENARIOS_OBJS) $(LIST_INSTRUMENTS_OBJS) src/main.o $(TARGETS)

test: all
	@echo "Starting UDP receiver in background..."
	@./udp_receiver &
	@sleep 1
	@echo "Running UDP publisher test..."
	@./test_udp_publisher
	@echo "Stopping receiver..."
	@pkill -f udp_receiver || true

.PHONY: all clean test

debug_snapshot_client: test/debug_snapshot_client.o $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
