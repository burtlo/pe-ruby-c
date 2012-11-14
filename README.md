## Particle Emitter

This is an attempt to port [71 Squared's Particle Designer] to Ruby.

**This only works on a MacOSX because of a CoreGraphics requirement.**

## Troubleshooting.

The code is currently not working. A blank, black screen appears when the code is run. 

Anything and everything could be wrong with the codebase. I want to point out some areas where I know that I have made assumptions.

### Particle Emitter Code

The code is an attempt to be a direct port of the [Particle Emitter project](http://particledesigner.71squared.com/?download=1) that parses and generates the particle emitters.

I know that Particle Emitter uses OpenGLES and the iPhone so I may be making some assumptions about setup, rendering or teardown. As this project is using [Gosu](http://www.libgosu.org/) it may do things differently as far as setting up the OpenGL environment that I am not considering.

#### C Structures vs Objective-C

As this code is ported from Objective-C, I may have been making a mistake as far as:

* Calculating pointer math
* Assigning values
* Incrementing pointers

## Installation

* Requires Ruby 1.9.3.
* Bundler

```
gem install bundler
bundle install
```

## Usage

### Clean, Build, and Run

```
./clean.sh
./build.sh
ruby learn_test.rb
```

### Guard

To provide quick feedback, I setup guard to run the following code above on any change of a **c/h file**. It is not necessary to run and test the code.
