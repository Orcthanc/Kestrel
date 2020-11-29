cam1[Transform, Camera]
plane1[
	Transform,
	Mesh = "../res/Kestrel/res/models/Terrain4x4.obj",
	Mat = "../res/Kestrel/shader/basic",
	Color = ( 1.0, 0.0, 0.0 )]
plane2[
	Transform,
	Mesh = "../res/Kestrel/res/models/Terrain4x4.obj",
	Mat = "../res/Kestrel/shader/basic",
	Color = ( 0.0, 1.0, 0.0 )]

void cam1(){
	Transform.loc = ( 0.0, 0.0, 0.0 );
	Transform.scale = ( 1.0, 1.0, 1.0 );
}

void plane1(){
	Transform.loc = ( 0.0, 0.0, 0.0001 ) * frame * frame;
	Transform.scale = ( 1.0, 1.0, 1.0 ) * frame * frame;
	Transform.scale = ( 1.0, 1.0, 1.0 ) + (0.0001, 0.0001, 0.0 ) * frame * frame;
}

void plane2(){
	Transform.loc = ( 0.0, 0.0, 0.00010001 ) * frame * frame;
	Transform.scale = ( 1.0, 1.0, 1.0 ) + (0.0001, 0.0001, 0.0 ) * frame * frame;
}
