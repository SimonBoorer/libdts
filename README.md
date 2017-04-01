# libdts
A library for reading and writing Torque engine DTS files

# Usage
```
TSShape shape("myShape.dts");

TSShapeConstructor construct(&shape);
construct.AddCollisionDetail(-1, TSShapeConstructor::kBox, "bounds");

shape.WriteToFile("myShape.dts");
```
