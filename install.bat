@cd %~dp0 

@make show_prefix
@echo (For Windows 7/Vista: make sure to right-click and select "Run as administrator")
@echo (This script does not support installing the library into AVR Studio 5.)
@pause

@make install

@pause
