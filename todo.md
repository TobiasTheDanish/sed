Setup for zooming:

  - [x] Introduce scale to editor struct.
  - [x] Introduce function to in/decrease scale of editor.
  - [x] Make rendering use editor scale instead of hardcoded value.
  - [x] Use <CTRL +> to increase and <CTRL -> to decrease scale.
  - [x] Stop rendering of '+' and '-'

Setup for scrolling:

  - [x] Introduce top, bot, left, right (viewport) to editor struct.
  - [x] Render using the viewport
  - [x] Make cursor move the viewport when at the edge of the viewport.
  - [x] Make viewport more responsive

Setup for line numbers:

  - [x] Introduce padding around viewport
  - [x] Render current line number
  - [x] Render relative line numbers
  - [x] Stop rendering if file does not fill out viewport

Setup of multiple modes

  - [x] Add mode enum
  - [x] Add mode property to editor struct
  - [x] Function to change modes
  - [x] Handle input for different modes
  - [x] Display current mode below viewport

Setup for editor info row:

  - [x] Add info property to editor struct
  - [x] Display current mode
  - [x] Render row below viewport

Setup for opening new files:
  
  - [x] Function to load new file
  - [x] Function to open new buffer with an existing file 
  - [ ] Function to prompt user for filename
  - [ ] Function to view open buffers
  - [ ] Function to swap between open buffers
  - [ ] Handle input for prompting files
