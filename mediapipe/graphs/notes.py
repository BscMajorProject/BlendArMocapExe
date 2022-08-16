CALCULATOR_TO_OPTIONS = {
    'ConstantSidePacketCalculator':
        constant_side_packet_calculator_pb2.ConstantSidePacketCalculatorOptions,
    'ImageTransformationCalculator':
        image_transformation_calculator_pb2
        .ImageTransformationCalculatorOptions,
    'LandmarksSmoothingCalculator':
        landmarks_smoothing_calculator_pb2.LandmarksSmoothingCalculatorOptions,
    'LogicCalculator':
        logic_calculator_pb2.LogicCalculatorOptions,
    'ThresholdingCalculator':
        thresholding_calculator_pb2.ThresholdingCalculatorOptions,
    'TensorsToDetectionsCalculator':
        tensors_to_detections_calculator_pb2
        .TensorsToDetectionsCalculatorOptions,
    'Lift2DFrameAnnotationTo3DCalculator':
        lift_2d_frame_annotation_to_3d_calculator_pb2
        .Lift2DFrameAnnotationTo3DCalculatorOptions,
}

class T:
 super().__init__(
        binary_graph_path=_BINARYPB_FILE_PATH,
        side_inputs={
            'model_complexity': model_complexity,
            'num_hands': max_num_hands,
            'use_prev_landmarks': not static_image_mode,
        },
        calculator_params={
            'palmdetectioncpu__TensorsToDetectionsCalculator.min_score_thresh':
                min_detection_confidence,
            'handlandmarkcpu__ThresholdingCalculator.threshold':
                min_tracking_confidence,
        },
        outputs=[
            'multi_hand_landmarks', 'multi_hand_world_landmarks',
            'multi_handedness'
        ])
