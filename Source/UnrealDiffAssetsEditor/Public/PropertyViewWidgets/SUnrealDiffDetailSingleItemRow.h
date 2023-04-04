﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SUnrealDiffDetailTableRowBase.h"

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SUnrealDiffDetailSingleItemRow : public SUnrealDiffDetailTableRowBase
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffDetailSingleItemRow) {}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TSharedRef<class FUnrealDiffDetailTreeNode> InOwnerTreeNode);

	TWeakPtr<class FUnrealDiffDetailTreeNode> OwnerTreeNode;

	virtual void OnExpanderClicked(bool bIsExpanded) override;

	virtual int32 GetIndentLevelForBackgroundColor() override;
protected:
	FSlateColor GetOuterBackgroundColor() const;
	FSlateColor GetInnerBackgroundColor() const;
	bool IsHighlighted() const;
};
