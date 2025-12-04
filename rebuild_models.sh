set -eux

cd include/model

python ~/model_generator2/generate_indexed_cpp2.py paddle.obj paddle > paddle.h
python ~/model_generator2/generate_indexed_cpp2.py block.obj block > block.h
python ~/model_generator2/generate_indexed_cpp2.py ball.obj ball > ball.h

rm -f *.mtl
