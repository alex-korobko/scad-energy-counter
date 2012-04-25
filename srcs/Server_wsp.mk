.PHONY: clean All

All:
	@echo ----------Building project:[ electricity_meter - Debug ]----------
	@cd "electricity_meter" && "$(MAKE)" -f "electricity_meter.mk"
clean:
	@echo ----------Cleaning project:[ electricity_meter - Debug ]----------
	@cd "electricity_meter" && "$(MAKE)" -f "electricity_meter.mk" clean
