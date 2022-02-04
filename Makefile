CC = gcc -Wall
OBJECTS_1 = utils.o trip.o cjson.o waypoint.o api_caller.o bot.o email_sender.o
OBJECTS_2 = csv_db_parser.o json_api_parser.o vehicle.o cfg_parser.o
OBJECTS = bbcbot.o $(OBJECTS_1) $(OBJECTS_2)

bbcbot: $(OBJECTS)
	$(CC) $(OBJECTS) -o bbcbot -lcurl

bbcbot.o: src/bbcbot.c
	$(CC) -c src/bbcbot.c -o bbcbot.o -I./lib

bot.o: src/bot.c src/bot.h
	$(CC) -c src/bot.c -o bot.o -I./lib

utils.o: src/utils.c src/utils.h
	$(CC) -c src/utils.c -o utils.o

trip.o: src/trip.c src/trip.h
	$(CC) -c src/trip.c -o trip.o

vehicle.o: src/vehicle.c src/vehicle.h
	$(CC) -c src/vehicle.c -o vehicle.o

waypoint.o: src/waypoint.c src/waypoint.h
	$(CC) -c src/waypoint.c -o waypoint.o

email_sender.o: src/email_sender.c src/email_sender.h
	$(CC) -c src/email_sender.c -o email_sender.o

cfg_parser.o: src/cfg_parser.c src/csv_db_parser.h
	$(CC) -c src/cfg_parser.c -o cfg_parser.o

csv_db_parser.o: src/csv_db_parser.c src/csv_db_parser.h
	$(CC) -c src/csv_db_parser.c -o csv_db_parser.o

json_api_parser.o: src/json_api_parser.c src/json_api_parser.h
	$(CC) -c src/json_api_parser.c -o json_api_parser.o -I./lib/

api_caller.o: src/api_caller.c src/api_caller.h
	$(CC) -c src/api_caller.c -o api_caller.o

cjson.o: lib/cJSON.c lib/cJSON.h
	$(CC) -c lib/cJSON.c -o cjson.o -I./lib/

clean:
	rm -f *.o main