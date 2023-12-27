For this project I have fully met the specification that is required in the instructions.
Each new map will be generated with the corresponding gates to previosuly loaded maps if there were any.
You can move north, south, east, west with corresponding key inputs n, s, e, w.
You can also fly around the map using the f input followed by 2 integers with the bounds of those integers being greater than -201 and less than 201.
To quit the program all you need to do is hit q and the program will exit on its own.
If you move in a direction that you are not supposed to or you input an operation that is invalid, you will get an error message with the same map printed again as well as the prompts.
I added a postSpawnMap method which is used to generate all maps after the spawn map.
This function gives a chance as to whether or not pokemarts and pokecenters spawn the farther you are from the spawn location.
The main program has a switch case nested in a while loop in order to keep the program running until you want to exit.