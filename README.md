# FG-EndlessRunner

This is my endless runner where you dodge ground obstacles and (soon) enemy projectiles.

Find my binary under the Releases tab as a ZIP

## Part 1

In this first pass at the endless runner game, I have used object-oriented design to determine which class objects to create in my game. I have achieved a loose coupling of the classes by:

- Using polymorphism where there is a BaseObstacle class that handles collision and making other obstacle classes like ShortObstacle inherit from it, being able to create different blueprints.

I also used Unreal's container classes and methods to:

- Store a list of ground tiles so they can be pooled instead of destroyed.

- Store a list of obstacles for each tile so they can be destroyed and re-spawned differently when the tile is recycled.

Known Bugs/Issues:

- Jumping doesn't work even if the character is grounded.

- The character won't think it's grounded in the build when it does in the engine, so jumping can't be tested.

- Sometimes the build will run at very low FPS. Waiting for a while fixed this. Not sure what causes it since it works fine in the engine. Shader compilation?

- Center lane obstacles have been removed since the player can't jump.

## Part 2

Not done yet.
 
