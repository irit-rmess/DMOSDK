CUR_PATH=$PWD
EXT_PATH=$CUR_PATH/../external
cd $EXT_PATH/CMSIS/doc && ./generate_html_doc.sh
cd $EXT_PATH/deca_driver/doc && ./generate_html_doc.sh
cd $EXT_PATH/FreeRTOS/doc && ./generate_html_doc.sh
cd $EXT_PATH/nrfx/doc && ./generate_html_doc.sh
cd $CUR_PATH && ./generate_html_doc.sh
