#include "ParametersWidget.h"

#include <QComboBox>
#include <QGroupBox>
#include <QLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QCheckBox>

ParametersWidget::ParametersWidget(QWidget* parent) :
	QWidget(parent)
{
	QVBoxLayout* layoutAlgorithmParams = new QVBoxLayout(); 

	comboboxGridSize_ = new QComboBox(this);
	comboboxGridSize_->insertItem(0, "17x17");
	comboboxGridSize_->insertItem(1, "33x33");
	setFixedWidth(300);
	QLabel* label = new QLabel(tr("Grid size:"), this);
	
	QHBoxLayout* layoutGridSize = new QHBoxLayout();
	layoutGridSize->addWidget(label);
	layoutGridSize->addWidget(comboboxGridSize_);

	QVBoxLayout* layoutAlgorithmApprox = new QVBoxLayout();
	layoutAlgorithmApprox->addLayout(layoutGridSize);

	spinboxPeakNeighborhoodGlobal_ = new QSpinBox(this);
	spinboxPeakNeighborhoodGlobal_->setRange(5, 1000);
	spinboxPeakNeighborhoodGlobal_->setValue(300);

	QLabel* labelPeakNeighborhoodGlobal = new QLabel(tr("Peak neighborhood global (pxls):"), this);
	QHBoxLayout* layoutPeakNeighborhoodGlobal = new QHBoxLayout;
	layoutPeakNeighborhoodGlobal->addWidget(labelPeakNeighborhoodGlobal);
	layoutPeakNeighborhoodGlobal->addWidget(spinboxPeakNeighborhoodGlobal_);

	layoutAlgorithmApprox->addLayout(layoutPeakNeighborhoodGlobal);

	QGroupBox* groupboxAlgorithmsApprox = new QGroupBox(tr("Algorithm approx. parameters"), this);
	groupboxAlgorithmsApprox->setLayout(layoutAlgorithmApprox);

//	layoutAlgorithmParams->addLayout(layoutGridSize);

	spinboxCellSizeFactor_ = new QDoubleSpinBox(this);
	spinboxCellSizeFactor_->setRange(0.1, 1.5);
	QLabel* labelCellSizeFactor = new QLabel(tr("Cell size factor:"), this);
	QHBoxLayout* layoutCellSizeFactor = new QHBoxLayout();
	layoutCellSizeFactor->addWidget(labelCellSizeFactor);
	layoutCellSizeFactor->addWidget(spinboxCellSizeFactor_);

	//layoutAlgorithmParams->addLayout(layoutCellSizeFactor);

	spinboxMaxPosError_ = new QSpinBox(this);
	spinboxMaxPosError_->setRange(1, 100);
	QLabel* labelMaxPosError = new QLabel(tr("Max pos. error (%)"), this);
	QHBoxLayout* layoutMaxPosError = new QHBoxLayout;
	layoutMaxPosError->addWidget(labelMaxPosError);
	layoutMaxPosError->addWidget(spinboxMaxPosError_);

	//layoutAlgorithmParams->addLayout(layoutMaxPosError);

	QVBoxLayout* layoutPositioningParams = new QVBoxLayout;
	layoutPositioningParams->addLayout(layoutCellSizeFactor);
	layoutPositioningParams->addLayout(layoutMaxPosError);

	QGroupBox* groupboxPositioningParams = new QGroupBox(tr("Positioning"), this);
	groupboxPositioningParams->setLayout(layoutPositioningParams);

	layoutAlgorithmParams->addWidget(groupboxPositioningParams);

	spinboxBlurImage_ = new QSpinBox(this);
	spinboxBlurImage_->setRange(0, 30);
	QLabel* labelBlurImage = new QLabel(tr("Image blur (pixels):"), this);
	QHBoxLayout* layoutBlurImage = new QHBoxLayout();
	layoutBlurImage->addWidget(labelBlurImage);
	layoutBlurImage->addWidget(spinboxBlurImage_);

	spinboxBlurMask_ = new QSpinBox(this);
	spinboxBlurMask_->setRange(0, 30);
	QLabel* labelBlurMask = new QLabel(tr("Mask blur (pixels):"), this);
	QHBoxLayout* layoutBlurMask = new QHBoxLayout();
	layoutBlurMask->addWidget(labelBlurMask);
	layoutBlurMask->addWidget(spinboxBlurMask_);

	comboboxSmoothingAlgorithm_ = new QComboBox(this);
	comboboxSmoothingAlgorithm_->insertItem(0, tr("Gaussian"));
	comboboxSmoothingAlgorithm_->insertItem(1, tr("Median"));

	QLabel* labelSmoothingAlgorithm = new QLabel(tr("Smoothing algorithm: "),this);

	QHBoxLayout* layoutSmoothingAlgorithm = new QHBoxLayout;
	layoutSmoothingAlgorithm->addWidget(labelSmoothingAlgorithm);
	layoutSmoothingAlgorithm->addWidget(comboboxSmoothingAlgorithm_);

	QVBoxLayout* layoutSmoothParams = new QVBoxLayout;
	layoutSmoothParams->addLayout(layoutSmoothingAlgorithm);
	layoutSmoothParams->addLayout(layoutBlurImage);
	layoutSmoothParams->addLayout(layoutBlurMask);

	QGroupBox* groupboxSmoothingParams = new QGroupBox(tr("Smoothing"), this);
	groupboxSmoothingParams->setLayout(layoutSmoothParams);

	layoutAlgorithmParams->addWidget(groupboxSmoothingParams);

	QVBoxLayout* layoutCalculationParams = new QVBoxLayout;

	QVBoxLayout* layoutLineThicknessSearch = new QVBoxLayout();

	checkboxThresholdEnabled_ = new QCheckBox(tr("Enable threshold"), this);
	
	layoutLineThicknessSearch->addWidget(checkboxThresholdEnabled_);

	QGroupBox* groupboxLineThicknessSearch = new QGroupBox(tr("Line thickness search"));

	spinboxThresholdValue_ = new QSpinBox(this);
	spinboxThresholdValue_->setRange(1, 255);
	QLabel* labelThresholdValue = new QLabel(tr("Threshold value:"), this);
	QHBoxLayout* layoutThresholdValue = new QHBoxLayout;

	checkboxAutoThresholdEnabled_ = new QCheckBox(tr("Auto threshold"), this);

	layoutThresholdValue->addWidget(checkboxAutoThresholdEnabled_);
	layoutThresholdValue->addWidget(labelThresholdValue);
	layoutThresholdValue->addWidget(spinboxThresholdValue_);

	layoutLineThicknessSearch->addLayout(layoutThresholdValue);

	spinboxPeakNeighborhoodLocal_ = new QSpinBox(this);
	spinboxPeakNeighborhoodLocal_->setRange(1, 500);
	spinboxPeakNeighborhoodLocal_->setValue(5);

	QLabel* labelPeakNeighborhoodLocal = new QLabel(tr("Peak neighborhood local (pxls):"), this);
	QHBoxLayout* layoutPeakNeighborhoodLocal = new QHBoxLayout;
	layoutPeakNeighborhoodLocal->addWidget(labelPeakNeighborhoodLocal);
	layoutPeakNeighborhoodLocal->addWidget(spinboxPeakNeighborhoodLocal_);

	layoutLineThicknessSearch->addLayout(layoutPeakNeighborhoodLocal);

	groupboxLineThicknessSearch->setLayout(layoutLineThicknessSearch);

	layoutAlgorithmParams->addWidget(groupboxLineThicknessSearch);

	QGroupBox* groupBoxAlgorithmParams = new QGroupBox(tr("Algorithm accurate parameters"), this);
	groupBoxAlgorithmParams->setLayout(layoutAlgorithmParams);


	QVBoxLayout* layoutMain = new QVBoxLayout;
	layoutMain->addWidget(groupboxAlgorithmsApprox);
	layoutMain->addWidget(groupBoxAlgorithmParams);
	//layoutMain->addWidget(groupboxLineThicknessSearch);
//	layoutMain->addWidget(groupboxSmoothingParams);

	layoutMain->addStretch();
	setLayout(layoutMain);

	connect(comboboxGridSize_, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, [=](int index) { updateParameters(); });
	connect(comboboxSmoothingAlgorithm_, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, [=](int index) { updateParameters(); });
	connect(spinboxCellSizeFactor_, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=](int index) { updateParameters(); });
	connect(spinboxBlurImage_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int index) { updateParameters(); });
	connect(spinboxBlurMask_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int index) { updateParameters(); });
	connect(spinboxMaxPosError_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int index) { updateParameters(); });
	connect(spinboxPeakNeighborhoodGlobal_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int index) { updateParameters(); });
	connect(spinboxPeakNeighborhoodLocal_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int index) { updateParameters(); });

	connect(checkboxThresholdEnabled_, &QCheckBox::toggled, this, [=](bool) { updateParameters(); });
	connect(checkboxAutoThresholdEnabled_, &QCheckBox::toggled, this, [=](bool) { updateParameters(); });
	connect(spinboxThresholdValue_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int index) { updateParameters(); });

	connect(checkboxAutoThresholdEnabled_, &QCheckBox::clicked, [=](bool checked) { spinboxThresholdValue_->setDisabled(checked); });
	//connect(checkboxThresholdEnabled_, &QCheckBox::clicked, [=](bool checked) {checkboxAutoThresholdEnabled_->setEnabled(checked); spinboxThresholdValue_->setEnabled(checked && !checkboxAutoThresholdEnabled_->isEnabled()); });
}

void ParametersWidget::updateParameters()
{
	Parameters params = getParameters();
	emit parametersChangedS(params);
}

void ParametersWidget::setParameters(const Parameters& params)
{
	if (params.gridCols == 33 && params.gridRows == 33)
	{
		comboboxGridSize_->setCurrentIndex(GRID_SIZE_33x33);
	}
	else if (params.gridCols == 17 && params.gridRows == 17)
	{
		comboboxGridSize_->setCurrentIndex(GRID_SIZE_17x17);
	}
	else
	{
		Q_ASSERT(false);
	}

	if (params.smoothingAlgorithm == SMOOTHING_GAUSS)
	{
		comboboxSmoothingAlgorithm_->setCurrentIndex(SMOOTHING_GAUSS);
	}
	else if (params.smoothingAlgorithm == SMOOTHING_MEDIAN)
	{
		comboboxSmoothingAlgorithm_->setCurrentIndex(SMOOTHING_MEDIAN);
	}
	else
	{
		Q_ASSERT(false);
	}

	blockAllSignals(true);
	spinboxCellSizeFactor_->setValue(params.cellSizeFactor);
	spinboxBlurImage_->setValue(params.blurImageSize);
	spinboxBlurMask_->setValue(params.blurMaskSize);
	spinboxMaxPosError_->setValue(params.maxPosError);
	spinboxPeakNeighborhoodGlobal_->setValue(params.peakNeighGlobal);
	spinboxPeakNeighborhoodLocal_->setValue(params.peakNeighLocal);
	checkboxThresholdEnabled_->setChecked(params.thresholdEnabled);
	checkboxAutoThresholdEnabled_->setChecked(params.autoThresholdEnabled);
	spinboxThresholdValue_->setValue(params.thresholdValue);
	blockAllSignals(false);
}

Parameters ParametersWidget::getParameters()
{
	Parameters params;
	const int c_index_grid_size = comboboxGridSize_->currentIndex();
	if (c_index_grid_size == GRID_SIZE_17x17)
	{
		params.gridRows = 17;
		params.gridCols = 17;
	}
	else if (c_index_grid_size == GRID_SIZE_33x33)
	{
		params.gridRows = 33;
		params.gridCols = 33;
	}

	const int c_index_smooth_algorithm = comboboxSmoothingAlgorithm_->currentIndex();
	if (c_index_smooth_algorithm == SMOOTHING_GAUSS)
	{
		params.smoothingAlgorithm = SMOOTHING_GAUSS;
	}
	else if (c_index_smooth_algorithm == SMOOTHING_MEDIAN)
	{
		params.smoothingAlgorithm = SMOOTHING_MEDIAN;
	}

	params.cellSizeFactor = spinboxCellSizeFactor_->value();
	params.blurImageSize = spinboxBlurImage_->value();
	params.blurMaskSize = spinboxBlurMask_->value();
	params.maxPosError = spinboxMaxPosError_->value();
	params.peakNeighGlobal = spinboxPeakNeighborhoodGlobal_->value();
	params.peakNeighLocal = spinboxPeakNeighborhoodLocal_->value();
	params.thresholdEnabled = checkboxThresholdEnabled_->isChecked();
	params.autoThresholdEnabled = checkboxAutoThresholdEnabled_->isChecked();
	params.thresholdValue = spinboxThresholdValue_->value();
	return params;
}

void ParametersWidget::blockAllSignals(bool block)
{
	spinboxCellSizeFactor_->blockSignals(block);
	spinboxBlurImage_->blockSignals(block);
	spinboxBlurMask_->blockSignals(block);
	spinboxMaxPosError_->blockSignals(block);
	spinboxPeakNeighborhoodGlobal_->blockSignals(block);
	spinboxPeakNeighborhoodLocal_->blockSignals(block);
	spinboxThresholdValue_->blockSignals(block);
	checkboxThresholdEnabled_->blockSignals(block);
	checkboxAutoThresholdEnabled_->blockSignals(block);
}

ParametersWidget::~ParametersWidget()
{

}