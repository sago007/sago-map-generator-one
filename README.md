# sago-map-generator-one
Generate OpenArena space maps

This is a proof of concept of a map generator for OpenArena. At the moment it simply creates some randomly distributed platforms. No skybox, no item, no player start.
It requires a lot of work in NetRadiant afterwards if you actually want to play it.
It does open the possibility to have larger maps that are still interesting.

The code is very rough. It is on the proof of concept phase and I needed the proof of concept fast.

## Running

Example 1:
```
./sago-map-generator-one --output sago-map-generated.map --numberOfLayers 10
```

For a full command list:
```
./sago-map-generator-one --help
```


## Compiling

```
cmake .
make
```

## Reading material

Procedurally generated content can be an interesting topic. Here are some inspiration:

  * https://gamedevelopment.tutsplus.com/tutorials/create-a-procedurally-generated-dungeon-cave-system--gamedev-10099
