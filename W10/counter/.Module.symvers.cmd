cmd_/home/ho/workspace/Emb_sys/W10/counter/Module.symvers := sed 's/\.ko$$/\.o/' /home/ho/workspace/Emb_sys/W10/counter/modules.order | scripts/mod/modpost -m -a  -o /home/ho/workspace/Emb_sys/W10/counter/Module.symvers -e -i Module.symvers   -T -
