##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=electricity_meter
ConfigurationName      :=Debug
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
WorkspacePath          := "/home/kor/scad-energy-counter/srcs"
ProjectPath            := "/home/kor/scad-energy-counter/srcs/electricity_meter"
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Alex
Date                   :=06/08/2012
CodeLitePath           :="/home/kor/.codelite"
LinkerName             :=g++
ArchiveTool            :=ar rcus
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
CompilerName           :=g++
C_CompilerName         :=gcc
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="/home/kor/scad-energy-counter/srcs/electricity_meter/electricity_meter.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
CmpOptions             := -g $(Preprocessors)
C_CmpOptions           := -g $(Preprocessors)
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)/usr/include/mysql 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)pthread $(LibrarySwitch)mysqlclient 
LibPath                := $(LibraryPathSwitch). 


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects=$(IntermediateDirectory)/main$(ObjectSuffix) $(IntermediateDirectory)/reciever$(ObjectSuffix) $(IntermediateDirectory)/configurator$(ObjectSuffix) $(IntermediateDirectory)/packets_processor$(ObjectSuffix) $(IntermediateDirectory)/logger$(ObjectSuffix) $(IntermediateDirectory)/my_sql_writer$(ObjectSuffix) $(IntermediateDirectory)/exception$(ObjectSuffix) $(IntermediateDirectory)/generic_socket$(ObjectSuffix) $(IntermediateDirectory)/server_socket$(ObjectSuffix) $(IntermediateDirectory)/mysql_writer_factory$(ObjectSuffix) \
	$(IntermediateDirectory)/modem$(ObjectSuffix) $(IntermediateDirectory)/scheduled_actions$(ObjectSuffix) $(IntermediateDirectory)/scheduled_action$(ObjectSuffix) $(IntermediateDirectory)/scheduled_action_read_data$(ObjectSuffix) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects) > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/kor/scad-energy-counter/srcs/electricity_meter/main.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/main$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main$(DependSuffix): main.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main$(ObjectSuffix) -MF$(IntermediateDirectory)/main$(DependSuffix) -MM "/home/kor/scad-energy-counter/srcs/electricity_meter/main.cpp"

$(IntermediateDirectory)/main$(PreprocessSuffix): main.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main$(PreprocessSuffix) "/home/kor/scad-energy-counter/srcs/electricity_meter/main.cpp"

$(IntermediateDirectory)/reciever$(ObjectSuffix): reciever.cpp $(IntermediateDirectory)/reciever$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/kor/scad-energy-counter/srcs/electricity_meter/reciever.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/reciever$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/reciever$(DependSuffix): reciever.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/reciever$(ObjectSuffix) -MF$(IntermediateDirectory)/reciever$(DependSuffix) -MM "/home/kor/scad-energy-counter/srcs/electricity_meter/reciever.cpp"

$(IntermediateDirectory)/reciever$(PreprocessSuffix): reciever.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/reciever$(PreprocessSuffix) "/home/kor/scad-energy-counter/srcs/electricity_meter/reciever.cpp"

$(IntermediateDirectory)/configurator$(ObjectSuffix): configurator.cpp $(IntermediateDirectory)/configurator$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/kor/scad-energy-counter/srcs/electricity_meter/configurator.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/configurator$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/configurator$(DependSuffix): configurator.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/configurator$(ObjectSuffix) -MF$(IntermediateDirectory)/configurator$(DependSuffix) -MM "/home/kor/scad-energy-counter/srcs/electricity_meter/configurator.cpp"

$(IntermediateDirectory)/configurator$(PreprocessSuffix): configurator.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/configurator$(PreprocessSuffix) "/home/kor/scad-energy-counter/srcs/electricity_meter/configurator.cpp"

$(IntermediateDirectory)/packets_processor$(ObjectSuffix): packets_processor.cpp $(IntermediateDirectory)/packets_processor$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/kor/scad-energy-counter/srcs/electricity_meter/packets_processor.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/packets_processor$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/packets_processor$(DependSuffix): packets_processor.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/packets_processor$(ObjectSuffix) -MF$(IntermediateDirectory)/packets_processor$(DependSuffix) -MM "/home/kor/scad-energy-counter/srcs/electricity_meter/packets_processor.cpp"

$(IntermediateDirectory)/packets_processor$(PreprocessSuffix): packets_processor.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/packets_processor$(PreprocessSuffix) "/home/kor/scad-energy-counter/srcs/electricity_meter/packets_processor.cpp"

$(IntermediateDirectory)/logger$(ObjectSuffix): logger.cpp $(IntermediateDirectory)/logger$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/kor/scad-energy-counter/srcs/electricity_meter/logger.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/logger$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/logger$(DependSuffix): logger.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/logger$(ObjectSuffix) -MF$(IntermediateDirectory)/logger$(DependSuffix) -MM "/home/kor/scad-energy-counter/srcs/electricity_meter/logger.cpp"

$(IntermediateDirectory)/logger$(PreprocessSuffix): logger.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/logger$(PreprocessSuffix) "/home/kor/scad-energy-counter/srcs/electricity_meter/logger.cpp"

$(IntermediateDirectory)/my_sql_writer$(ObjectSuffix): my_sql_writer.cpp $(IntermediateDirectory)/my_sql_writer$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/kor/scad-energy-counter/srcs/electricity_meter/my_sql_writer.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/my_sql_writer$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/my_sql_writer$(DependSuffix): my_sql_writer.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/my_sql_writer$(ObjectSuffix) -MF$(IntermediateDirectory)/my_sql_writer$(DependSuffix) -MM "/home/kor/scad-energy-counter/srcs/electricity_meter/my_sql_writer.cpp"

$(IntermediateDirectory)/my_sql_writer$(PreprocessSuffix): my_sql_writer.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/my_sql_writer$(PreprocessSuffix) "/home/kor/scad-energy-counter/srcs/electricity_meter/my_sql_writer.cpp"

$(IntermediateDirectory)/exception$(ObjectSuffix): exception.cpp $(IntermediateDirectory)/exception$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/kor/scad-energy-counter/srcs/electricity_meter/exception.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/exception$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/exception$(DependSuffix): exception.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/exception$(ObjectSuffix) -MF$(IntermediateDirectory)/exception$(DependSuffix) -MM "/home/kor/scad-energy-counter/srcs/electricity_meter/exception.cpp"

$(IntermediateDirectory)/exception$(PreprocessSuffix): exception.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/exception$(PreprocessSuffix) "/home/kor/scad-energy-counter/srcs/electricity_meter/exception.cpp"

$(IntermediateDirectory)/generic_socket$(ObjectSuffix): generic_socket.cpp $(IntermediateDirectory)/generic_socket$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/kor/scad-energy-counter/srcs/electricity_meter/generic_socket.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/generic_socket$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/generic_socket$(DependSuffix): generic_socket.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/generic_socket$(ObjectSuffix) -MF$(IntermediateDirectory)/generic_socket$(DependSuffix) -MM "/home/kor/scad-energy-counter/srcs/electricity_meter/generic_socket.cpp"

$(IntermediateDirectory)/generic_socket$(PreprocessSuffix): generic_socket.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/generic_socket$(PreprocessSuffix) "/home/kor/scad-energy-counter/srcs/electricity_meter/generic_socket.cpp"

$(IntermediateDirectory)/server_socket$(ObjectSuffix): server_socket.cpp $(IntermediateDirectory)/server_socket$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/kor/scad-energy-counter/srcs/electricity_meter/server_socket.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/server_socket$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/server_socket$(DependSuffix): server_socket.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/server_socket$(ObjectSuffix) -MF$(IntermediateDirectory)/server_socket$(DependSuffix) -MM "/home/kor/scad-energy-counter/srcs/electricity_meter/server_socket.cpp"

$(IntermediateDirectory)/server_socket$(PreprocessSuffix): server_socket.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/server_socket$(PreprocessSuffix) "/home/kor/scad-energy-counter/srcs/electricity_meter/server_socket.cpp"

$(IntermediateDirectory)/mysql_writer_factory$(ObjectSuffix): mysql_writer_factory.cpp $(IntermediateDirectory)/mysql_writer_factory$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/kor/scad-energy-counter/srcs/electricity_meter/mysql_writer_factory.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/mysql_writer_factory$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/mysql_writer_factory$(DependSuffix): mysql_writer_factory.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/mysql_writer_factory$(ObjectSuffix) -MF$(IntermediateDirectory)/mysql_writer_factory$(DependSuffix) -MM "/home/kor/scad-energy-counter/srcs/electricity_meter/mysql_writer_factory.cpp"

$(IntermediateDirectory)/mysql_writer_factory$(PreprocessSuffix): mysql_writer_factory.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/mysql_writer_factory$(PreprocessSuffix) "/home/kor/scad-energy-counter/srcs/electricity_meter/mysql_writer_factory.cpp"

$(IntermediateDirectory)/modem$(ObjectSuffix): modem.cpp $(IntermediateDirectory)/modem$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/kor/scad-energy-counter/srcs/electricity_meter/modem.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/modem$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/modem$(DependSuffix): modem.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/modem$(ObjectSuffix) -MF$(IntermediateDirectory)/modem$(DependSuffix) -MM "/home/kor/scad-energy-counter/srcs/electricity_meter/modem.cpp"

$(IntermediateDirectory)/modem$(PreprocessSuffix): modem.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/modem$(PreprocessSuffix) "/home/kor/scad-energy-counter/srcs/electricity_meter/modem.cpp"

$(IntermediateDirectory)/scheduled_actions$(ObjectSuffix): scheduled_actions.cpp $(IntermediateDirectory)/scheduled_actions$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/kor/scad-energy-counter/srcs/electricity_meter/scheduled_actions.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/scheduled_actions$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/scheduled_actions$(DependSuffix): scheduled_actions.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/scheduled_actions$(ObjectSuffix) -MF$(IntermediateDirectory)/scheduled_actions$(DependSuffix) -MM "/home/kor/scad-energy-counter/srcs/electricity_meter/scheduled_actions.cpp"

$(IntermediateDirectory)/scheduled_actions$(PreprocessSuffix): scheduled_actions.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/scheduled_actions$(PreprocessSuffix) "/home/kor/scad-energy-counter/srcs/electricity_meter/scheduled_actions.cpp"

$(IntermediateDirectory)/scheduled_action$(ObjectSuffix): scheduled_action.cpp $(IntermediateDirectory)/scheduled_action$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/kor/scad-energy-counter/srcs/electricity_meter/scheduled_action.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/scheduled_action$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/scheduled_action$(DependSuffix): scheduled_action.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/scheduled_action$(ObjectSuffix) -MF$(IntermediateDirectory)/scheduled_action$(DependSuffix) -MM "/home/kor/scad-energy-counter/srcs/electricity_meter/scheduled_action.cpp"

$(IntermediateDirectory)/scheduled_action$(PreprocessSuffix): scheduled_action.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/scheduled_action$(PreprocessSuffix) "/home/kor/scad-energy-counter/srcs/electricity_meter/scheduled_action.cpp"

$(IntermediateDirectory)/scheduled_action_read_data$(ObjectSuffix): scheduled_action_read_data.cpp $(IntermediateDirectory)/scheduled_action_read_data$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/kor/scad-energy-counter/srcs/electricity_meter/scheduled_action_read_data.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/scheduled_action_read_data$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/scheduled_action_read_data$(DependSuffix): scheduled_action_read_data.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/scheduled_action_read_data$(ObjectSuffix) -MF$(IntermediateDirectory)/scheduled_action_read_data$(DependSuffix) -MM "/home/kor/scad-energy-counter/srcs/electricity_meter/scheduled_action_read_data.cpp"

$(IntermediateDirectory)/scheduled_action_read_data$(PreprocessSuffix): scheduled_action_read_data.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/scheduled_action_read_data$(PreprocessSuffix) "/home/kor/scad-energy-counter/srcs/electricity_meter/scheduled_action_read_data.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/main$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/main$(DependSuffix)
	$(RM) $(IntermediateDirectory)/main$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/reciever$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/reciever$(DependSuffix)
	$(RM) $(IntermediateDirectory)/reciever$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/configurator$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/configurator$(DependSuffix)
	$(RM) $(IntermediateDirectory)/configurator$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/packets_processor$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/packets_processor$(DependSuffix)
	$(RM) $(IntermediateDirectory)/packets_processor$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/logger$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/logger$(DependSuffix)
	$(RM) $(IntermediateDirectory)/logger$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/my_sql_writer$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/my_sql_writer$(DependSuffix)
	$(RM) $(IntermediateDirectory)/my_sql_writer$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/exception$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/exception$(DependSuffix)
	$(RM) $(IntermediateDirectory)/exception$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/generic_socket$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/generic_socket$(DependSuffix)
	$(RM) $(IntermediateDirectory)/generic_socket$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/server_socket$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/server_socket$(DependSuffix)
	$(RM) $(IntermediateDirectory)/server_socket$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/mysql_writer_factory$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/mysql_writer_factory$(DependSuffix)
	$(RM) $(IntermediateDirectory)/mysql_writer_factory$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/modem$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/modem$(DependSuffix)
	$(RM) $(IntermediateDirectory)/modem$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/scheduled_actions$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/scheduled_actions$(DependSuffix)
	$(RM) $(IntermediateDirectory)/scheduled_actions$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/scheduled_action$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/scheduled_action$(DependSuffix)
	$(RM) $(IntermediateDirectory)/scheduled_action$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/scheduled_action_read_data$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/scheduled_action_read_data$(DependSuffix)
	$(RM) $(IntermediateDirectory)/scheduled_action_read_data$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) "/home/kor/scad-energy-counter/srcs/.build-debug/electricity_meter"


