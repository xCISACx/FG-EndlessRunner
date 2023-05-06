# FG-EndlessRunner

This is my endless runner where you dodge ground obstacles and (soon) enemy projectiles.

Find my binary under the Releases tab as a ZIP

Controls:

Player 1:
Jump: W or Space.
Slide: S.
Move: A and D.

Player 2:
Jump: Up Arrow.
Slide: Down Arrow.
Move: Left and Right Arrows.

Pause: P

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

- Fixed jumping. cfd355285141868db0ac8874127f36f50b25ee83
- Added sliding. cfd355285141868db0ac8874127f36f50b25ee83
- Added UI. 22f5f18d4809b66e4c642598e69a80d47406b13a
- Added lives. 22f5f18d4809b66e4c642598e69a80d47406b13a
- Added score. 22f5f18d4809b66e4c642598e69a80d47406b13a
- Added reset. 22f5f18d4809b66e4c642598e69a80d47406b13a
- Added tall obstacles for the player to slide under. 22f5f18d4809b66e4c642598e69a80d47406b13a
- Added obstacles despawning when player successfully dodges. 22f5f18d4809b66e4c642598e69a80d47406b13a
- Added multiplayer. b1b3d2da4862dd8a358444e6ae50d5899b884e19
- Added High Score saving. b1b3d2da4862dd8a358444e6ae50d5899b884e19
- Added Main Menu. b1b3d2da4862dd8a358444e6ae50d5899b884e19
- Added Game Over condition. b1b3d2da4862dd8a358444e6ae50d5899b884e19
- Added Game Over screen. b1b3d2da4862dd8a358444e6ae50d5899b884e19
- Added Pause screen. b1b3d2da4862dd8a358444e6ae50d5899b884e19
- Cleaned up the code to remove debug messages and old code. b1ac4c41a0e48b400221bad54fbb069e5573121d
- Fixed clearing a tile not adding points to the score. b1ac4c41a0e48b400221bad54fbb069e5573121d
- Fixed missing reference to game over blueprint. de7f3f9a3d243d03dbb67a1256fa1bfcb066a710

Known Bug: On Game Over, the displayed Winner Score will be higher than the displayed Player Score at the top, unsure why.
