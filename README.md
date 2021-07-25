# XMP-Toolkit-SDK

The XMP Toolkit allows you to integrate XMP functionality into your product or solution. It supports Macintosh, Windows, as well as UNIX and comes with samples, documentation, source code and scripts to generate project files. The XMP Toolkit is available under the BSD license. The specification is provided under the [XMP Specification Public Patent License](https://wwwimages2.adobe.com/content/dam/acom/en/devnet/xmp/pdfs/cs6/xmp_public_patent_license.pdf)


## SDK components

The XMP Toolkit SDK contains two libraries, XMPCore and XMPFiles. XMPCore and XMPFiles are provided as C++ implementations.

### XMPCore
This library supplies an API for parsing, manipulating, and serializing metadata, according to the XMP data model and regardless of the file format of the data it describes.

### XMPFiles
This library supplies an API for locating, adding, or updating the XMP metadata in a file. The API allows you to retrieve the entire XMP Packet, which you can then pass to the XMPCore component in order to manipulate the individual XMP properties.

*It also includes a plug-in SDK that allows you to create an XMPFiles Plug-in that handles metadata for additional file formats, or replaces built-in format handlers with custom ones.*

## XMP Toolkit SDK Contents
This git repo contains the following folders under the root folder:

Directory | Details 
------------ | -------------
/ 				| At the root level, the license agreement (BSD_License.txt) and this overview (XMP-Toolkit-SDK-Overview.pdf).
build/ 			| Contains Batch file, shell scripts, Makefile and CMake scripts to be used to create XMP Toolkit SDK project files on the supported platforms. Follow the instructions in README.txt to create the projects.
docs/ 			| The three-part XMP Specification, the XMP Toolkit SDK Programmer’s Guide, the API reference documentation (API/index.html) and the XMPFiles plug-in SDK documentation.
public/include/ | The header files and glue code that clients of the XMP Toolkit SDK must include.
samples/ 		| Sample source code and CMake scripts for building sample projects, with the necessary resources to run the sample code. See ‘‘Sample code and tools’’ below.
source/ 		| The common source code that is used by both components of the XMP Toolkit SDK.
XMPCore/ 		| The source code for XMPCore library.
XMPFiles/ 		| The source code for XMPFiles library.
third-party/ <br>&nbsp;&nbsp;&nbsp;expat/<br>&nbsp;&nbsp;&nbsp;zlib/<br>&nbsp;&nbsp;&nbsp;zuid/	| Placeholders for third party source files which are needed for the XMP Toolkit SDK, including ReadMe.txt files with information on how to obtain and install the tools. MD5 source code, needed by both components for MD5 hash computation, is included.
tools/ 			| Placeholder for downloading and placing CMake tool
XMPFilesPlugins/| The header files and glue code of the XMPFiles plug-in SDK and a sample plug-in.

## How to Build and Use

XMP Toolkit SDK provides build script to ease the process. This repository contains everything needed to build SDK libraries on Mac OS®, Windows®, UNIX®/Linux®, iOS and Android. For detailed build steps , refer the [Programmer's Guide](https://github.com/adobe/XMP-Toolkit-SDK/blob/master/docs/XMPProgrammersGuide.pdf)

## Documentation 
See extensive [API documentation](https://github.com/adobe/XMP-Toolkit-SDK/blob/master/docs/XMPProgrammersGuide.pdf) where a complete API Reference is available.

The XMP Specification, available from [Adobe Developer Center (XMP)](http://adobe.com/devnet/xmp/), provides a complete formal
specification for XMP. Before working with the XMP Toolkit SDK, you must be familiar with, at a minimum,
the XMP Data Model.

The specification has three parts:
- Part 1, Data Model, Serialization, and Core Properties covers the basic metadata representation model
that is the foundation of the XMP standard format. The Data Model prescribes how XMP metadata can
be organized; it is independent of file format or specific usage. The Serialization Model prescribes how
the Data Model is represented in XML, specifically RDF.
This document provides all the details a programmer would need to implement a metadata
manipulation system such as the XMP Toolkit SDK (which is available from Adobe).

- Part 2, Additional Properties, provides detailed property lists and descriptions for standard XMP
metadata schemas; these include general-purpose schemas such as Dublin Core, and special-purpose
schemas for Adobe applications such as Photoshop®. It also provides information on extending
existing schemas and creating new schemas.

- Part 3, Storage in Files, provides information about how serialized XMP metadata is packaged into XMP
Packets and embedded in different file formats. It includes information about how XMP relates to and
incorporates other metadata formats, and how to reconcile values that are represented in multiple
metadata formats.

## Contributing
Contributions are welcomed! Read the [Contributing Guide](https://github.com/adobe/XMP-Toolkit-SDK/blob/master/CONTRIBUTING.md) for more information.

## Licensing
This project is BSD licensed. See [LICENSE](https://github.com/adobe/XMP-Toolkit-SDK/blob/main/LICENSE) for more information.

## Report Issues/Bugs
You can report the issues in the issues section of the github repo.
