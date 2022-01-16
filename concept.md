# View

Orthographic camera aiming down

Layers (Top down)
1. Overlay
2. Fog of War
3. Tokens
4. Grid
5. Image
6. Background (Flat colour/checker)

Alternatively, layers could be a flat list with arbitrary ordering, but placement should have default values to group them.

## Overlay
Contains arbitrary elements to display over everything, eg, arrows, circles, writing, etc...
Elements are owned, and optionally shared. By default, overlay elements are only shown to the client that draws it.

## Fog Of War
Shader that generates an opaque range of vision based on the selected token. Should account for:
* Heightmap obstructions (could have a token "height" property...)
* Light (Global parameter + per token parameter ("emits light": distance))
* Darkvision (would need to be a token property)

## Tokens
All player and monster tokens in an array, drawn from first to last. Tokens have properties and can be influenced.

## Grid
Optional. Squares or hexes. Line thickness should be changeable.

## Image
Background image to be drawn. Could potentially be done as multiple tiles, but would need some planning so that each correct height map is used for the fog of war.

May want to have the option to have multiple levels on top of one another, each player only rendering the one their character is on. If they have multiple characters, eg, familiar, then they'd need to be able to toggle between them.

## Background
Flat colour / checker pattern to indicate there's no image here.

# Token Properties

* [S] Position
* [S] Name
* [S] Icon
* [S] Size
* [S] outerColour
* [S] tintColour
* [S] Description
* [S] Owner
* Hide Name
* Hide Description
* Max Vision Distance
* Darkvision

By default, only [S] properties are synced from host to client. "Hide" properties prevent their respective fields from being synced to clients (unsetting them if they have been shared).
Owner is just the name of the owner for rendering purposes, however, each client should have an ID that the host uses for rejecting requests to modify other clients tokens. See Networking section below.

Position should be a vec3, with z defaulting to 0. It could be used for vertical height if the token is considered to be flying.

Token may need to inherit from a base Shape class. Overlays could then use the Shape class to implement more arbitrary shapes, like lines, arrows, boxes, etc...

# Controls

Client controls:
* Any time:
    * Choose icon (used for player token)
    * Measure (Draws in overlay)
        * Distance
        * Radius (select center; position or token)
* Initiative Mode:
    * Plan move (draws in overlay, sharing optional)
    * Execute move
* Free Mode:
    * WASD movement (if player token is placed and not locked)
    * Switch Token (Switches which token is currently affected by the input)
    * Link Token(s) (Allows multiple tokens to be moved at once, eg, Amena and Kik)
* Share Control (specifies other clients that are allowed to modify this clients token(s))

Host controls:
* Select BG + Heightmap (might need to be a placement option)
* Place Token Mode
    * Token to place is selected from a pool on the side.
    * Player tokens can only be placed once.
* Interactive Mode
    * Select token(s)
    * Move token(s)
* Preview Token View (shows the Fog of War that would be generated for a token)
* Define trigger zone (on enter, all player input is locked)

* Enable/Disable Initiative Mode (locks player input, specific UI for selecting the current player)
* Start/Stop Sync (locks player input when not synced?)
* Set current view as Player Camera
    * View is saved and resent if a player connects after the change is made (allows scene setup)
* Blackout (players Fog of War is reset and not cleared until unchecked)
* Lock Player Input (prevents the player moving freely)
    * Default to "all players", add individual control later
* Save/Load Scene (needs a file format to save to)


# Networking
Application as host opens a TCP socket, Application as client connects to a host.
Sync commands are used to share data when changes are made, such as:
* Player icon changed
* Player moved
* Overlay (un)shared
* Token added/removed
* Background changed
* Player input locked
* Blackout

Could simply have a scene file format that is auto-saved, and whenever a local action would modify the format the patch is synced.
- How to handle conflicts?
    - Host has priority
    - Players shouldn't have options they can conflict with... yet
    - If players can conflict, will need a locking system, ie, first player request locks the token and the host rejects further requests to obtain the lock.

Clients should have a unique ID (host/client generated) that it submits with all sync requests. The host enforces permissions on sync requests based on this ID, eg, if the host thinks token A has clientID 12345 and clientID 67890 tries to modify it, the request is rejected. Clients will have to pre-emptively validate access before being allowed to modify a token for another client.

Will be a TCP connection, but for simplicity represented here as a HTML request. Note, for "Free move", a separate UDP connection may be needed.
```
PATCH /scenes/<id>/
{
    "edit": {
        "tokens": {
            "tokenID": {
                "position": [x, y]
            }
        }
    }
}
```

## Textures
Textures should be loaded and a checksum calculated. The TextureCache should be able to look up by path or checksum. Textures should be serialized separately and store the checksum and filepath together. Clients will receive the serialized scene, then compare the checksums against their local cache. Missing checksums can be requested from the host and then saved to the local cache.


# Architecture
Could potentially have the C++ handle OpenGL/UI and a python networking layer. The C++ API would only need to expose a way of updating it's scene file.


# Reference Notes

Great resource for how makefiles work (multiple pages): https://web.mit.edu/gnu/doc/html/make_1.html

Using imgui for UI: https://github.com/ocornut/imgui
with filesystem dialog: https://github.com/dfranx/ImFileDialog
    (simplified alternative: https://github.com/aiekick/ImGuiFileDialog)

Might consider switching to TinyEngine if more features are needed: https://github.com/weigert/TinyEngine

Infinite Grid: http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/

# TODO
- [ ] Separate Token and TokenInstance. TokenInstance is a unique model with shared Token. Promotable to individual Token. (Not really necessary now that batch edit is an option)
- [ ] Relative paths converted to absolute in TextureCache (less important as unlikely to be manually entering paths)
- [ ] Further investigation into shared/packed/std140 layouts in glsl
- [x] Snap to grid
- [x] UI sections (background, grid, tokens, etc...)
- [x] Drag select
- [x] Batch Edit
- [x] Add Token
- [x] Duplicate Token
- [x] Save/Load
- [ ] Undo Queue (Actions)
- [ ] Images not loading with correct sizes/distortion. Likely the wrong stbi options.
- [x] Background image offset
- [x] Clean up structure. Global shaders, Window logic separated, single quad mesh that's shared, InputManager. Proper memory management.
- [-] Overlays
- [ ] Fog of War
- [ ] Lighting effects
- [ ] Token statuses (multiple predefined effects that can be toggled)
- [x] Separate windows for viewport and UI (SetWindowSize to match framebuffer)
    - [ ] Hotkey to show/hide UI window
- [ ] scene file as CLI arg
- [ ] Esc confirm dialog (once separate UI windows, esc should at worst close the UI window automatically)
    - [ ] Maybe file dialogs closed by Esc if open
    - [ ] Focus UI window when prompting
- [ ] Initiative tracker?
- [ ] Token/Image re-ordering
- [ ] Token list
- [ ] Token library (pre-mades, single button to add, button uses token image and name)
- [ ] Constrain UI to UIWindow
