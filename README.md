# cvrt

A tiny conversion tool 


A small tool to convert given directory contents using an external tool, and then save the converted files to a new given directory tree respecting the same hierarchy of the original structure. 

The code will call an external command to conduct the conversion. 

This program is written in ANSI C11.

## Installation

1. To build:
* `%make all`
2. To clean
* `%make clean`
3. To install 
* `%sudo make install`
4. To Uninstall
* `%sudo make deinstall`

```
usage: bserver options
	[-b --base]	<BASE_FOLDER>
	[-d --dest]	<DESTINATION_FOLDER>
	[-f --conf]	<CONF_FILE>
	[-h --help]
	[-v --version]
```

**Default configuration file `~/.config/cvrt/config.ini`**

For handling ini file cvrt is using [ini module provided by rxi](https://github.com/rxi/ini) 

## Sample configuration file
```
[filter]
	; enable filtering by file type, if not enabled
	; then all the files will be converted using the
	; external command
	enabled = true
	; set the required file types, if filter is enabled
	; then this field is mandatory
	file_types = .wav .flac
	[copy]
	; if filter is enabled, then other files types
	; will be copied to the new location
	enabled = true
	; if copy is enabled, will copy the file types below
	; * or blank for all files other than
	; the above filtered ones
	file_types =

	[core]
	; external command to execute, ? will be replaced by variables
	base_dir = Music
	dest_dir = Music2
	cvrt_type = opus
	; External command to execute, expecting 2 '?'
	; '?'s are place-holders for input and output files.
	cmd = ffmpeg -i ? -c:a libopus ?
```
