cmake_minimum_required(VERSION 2.8)

set(GetSet_HEADERS "BaseTypes.hxx;StringType.hxx;StringConversion.hxx;StringHexData.hxx;StringUtil.hxx;GetSet.hxx;GetSet_impl.hxx;GetSetObjects.h;ProgressInterface.hxx;GetSetScripting.h;GetSetInternal.h;GetSetIO.h;GetSetLog.hxx;GetSetCmdLine.hxx")

add_library(GetSet
	${GetSet_HEADERS}
	GetSetObjects.cpp
	GetSetInternal.cpp
	GetSetIO.cpp
	GetSetScripting.cpp
)
set_target_properties(GetSet PROPERTIES VERSION ${GetSet_VERSION})

install(FILES ${GetSet_HEADERS} DESTINATION include/GetSet)
install(TARGETS GetSet EXPORT GetSetExport DESTINATION lib)

if (WIN32)
	install(FILES $<TARGET_FILE:GetSet> CONFIGURATIONS Debug   DESTINATION bin )
	install(FILES $<TARGET_FILE:GetSet> CONFIGURATIONS Release DESTINATION bin )
endif(WIN32)