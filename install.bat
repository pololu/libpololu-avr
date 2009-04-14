@FOR /F %%I IN ("%0") DO @SET BATDIR=%%~dpI
@cd %BATDIR% 

@make show_prefix
@echo (For Windows Vista: make sure to right-click and select "Run as administrator")
@pause

@make install

@echo Installation is complete.
@pause
