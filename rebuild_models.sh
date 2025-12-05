set -eux

cd include/model

python ~/model_generator2/generate_indexed_cpp2.py paddle.obj paddle > paddle.h
python ~/model_generator2/generate_indexed_cpp2.py block.obj block > block.h
python ~/model_generator2/generate_indexed_cpp2.py ball.obj ball > ball.h
python ~/model_generator2/generate_indexed_cpp2.py cube.obj cube > cube.h
python ~/model_generator2/generate_indexed_cpp2.py plane.obj plane > plane.h

rm -f *.mtl
