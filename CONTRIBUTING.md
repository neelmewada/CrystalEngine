# Contributing Guidelines

#### First off, thank you for considering contributing to Crystal Engine. I highly appreciate any contributions you make, no matter how small or big.

#### Following these guidelines helps to communicate that you respect the time of the developers managing and developing this open source project. In return, they should reciprocate that respect in addressing your issue, assessing changes, and helping you finalize your pull requests.

#### Keep an open mind! Improving documentation, bug tracking & fixing, or writing tutorials are all examples of helpful contributions that I'm looking for.

#### Crystal Engine is a work-in-progress modern vulkan based game engine with a focus on good foundation. The entire engine is divided into multiple modules in different domains.

#### Generally, I am not looking for external contributions on main engine features, like for example: a modification of the rendering engine, etc.

#### Again, defining this up front means less work for you. If someone ignores your guide and submits something you don’t want, you can simply close it and point to your policy.

## Responsibilities

* Ensure that you are not using a platform specific native feature directly in a module without any PAL abstractions. (PAL stands for Platform Abstraction Layer).
* You can look at several PAL folders inside the Core and CoreApplication modules for an example on how to abstract it.
* Ensure that any new features/bug fixes added to Core module are Unit tested using the Core_Test target in Visual studio.
* Don't add any classes to the codebase unless absolutely needed. Try adding methods first.
* Keep feature versions as small as possible, preferably one new feature per version.
* Be welcoming to newcomers and encourage diverse new contributors from all backgrounds.

## Getting started

You can clone the repo itself or create a fork of the main branch. Please refer to [Build.md](/Docs/Build.md) on instructions to build the project. For now, only Windows is supported. On MacOS, only the Core module compiles for now.

### How to contribute

1. Create your own fork of the main branch.
2. Make your changes to the code in your fork.
3. Make sure to run Core_Test unit test target if you make changes to the core module.
4. Send a pull request with a detailed explanation of your changes.

### Filing a bug report

When filing an issue, make sure to answer these five questions:
1. What branch and commit version you found the issue on?
2. What operating system and hardware specs you are using? OS version, CPU, GPU, System RAM, etc.
3. What did you do?
4. What did you expect to see?
5. What did you see instead?
6. Is the bug reproducible 100% of the times?

You can post the bug in Issue Tracker with the above details.

### How to request a feature

If you find yourself wishing for a feature that doesn't exist, then feel free to create an Issue ticket describing the feature in detail.

### Code review process

I will personally look at the pull requests and review it myself. I'll ask you to make changes if needed if I want your feature to be merged into main branch. If you don't respond to the change requests within 2 weeks, the pull request will be closed and won't be merged.

# Community

You can post your issues and feature requests in the Issue tracker. Further more, if you want to contact me, feel free to email neelrmewada@gmail.com

