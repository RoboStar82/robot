#! /usr/bin/env python3

from ultralytics import YOLO

model = YOLO("yolo26n.pt")

model.export(format="litert", quantize=8, data="coco8.yaml", imgsz=320)
