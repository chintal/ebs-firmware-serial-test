

FIND_PACKAGE("ucdm" REQUIRED)
FIND_PACKAGE("ds" REQUIRED)
FIND_PACKAGE("prbs" REQUIRED)

SET(application_SRCS main.c
                     application_descriptors.c
                     serial_test.c)
LIST(APPEND deps "hal-uc-core hal-uc-gpio hal-uc-uart hal-uc-usb hal-uc-id prbs")

ADD_PLATFORM_EXECUTABLE(firmware ${deps} ${application_SRCS})
