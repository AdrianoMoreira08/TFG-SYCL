# Introduction to high performance computing with SYCL

![TFG SYCL](logo_ull_dark.png#gh-light-mode-only "TFG SYCL")
![TFG SYCL](logo_ull_light.png#gh-dark-mode-only "TFG SYCL")

This repository compiles all the resources and code produced for the TFG (_Trabajo de Fin de Grado_) "Introduction to high performance computing with SYCL".

## What is this work about?

High Performance Computing (HPC) is a practice that utilizes powerful processors in parallel to process big data and solve complex problems at incredibly high speeds.
These systems operate at speeds considerably faster than regular systems.
Supercomputers, which incorporate millions of processors, have traditionally been the norm for HPC.

With the vast amount of available devices comes the also varied collection of APIs and other utilities regarding the use of both hardware and software.
This situation is problematic because a heterogeneous world requires heterogeneous solutions and tools.
This means that, in order to produce code that is targeted towards multiple back-ends or devices, there needs to be an abstraction that permits to do so, allowing multiple tools to work together.
This begs for a tool that provides a more generalistic yet time and performance efficacious approach. In response to this situation comes SYCL.

SYCL is a single source, high-level, standard C++ programming model, that can target a range of heterogeneous platforms.
There are many back-end implementations that support SYCL.
We are going to focus on the Intel DPC++ branch.
