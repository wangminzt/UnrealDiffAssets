﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SUnrealDiffDataTableListViewRow.h"

#include "SDataTableVisualDiff.h"
#include "SlateOptMacros.h"
#include "UnrealDiffAssetDelegate.h"
#include "DataTableWidgets/SUnrealDiffDataTableLayout.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"


#define LOCTEXT_NAMESPACE "SUnrealDiffDataTableListViewRow"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDataTableListViewRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	bIsLocal = InArgs._IsLocal;
	DataTableVisual = InArgs._DataTableVisual;
	DataTableLayout = InArgs._DataTableLayout;
	RowDataPtr = InArgs._InRowDataPtr;
	
	SMultiColumnTableRow<FUnrealDiffDataTableRowListViewDataPtr>::Construct(
		FSuperRowType::FArguments()
#if ENGINE_MAJOR_VERSION == 4
			.Style(FEditorStyle::Get(), "DataTableEditor.CellListViewRow"),
#else
			.Style(FAppStyle::Get(), "DataTableEditor.CellListViewRow"),
#endif
		InOwnerTableView
	);

	SetBorderImage(TAttribute<const FSlateBrush*>(this, &SUnrealDiffDataTableListViewRow::GetBorder));
}

const FSlateBrush* SUnrealDiffDataTableListViewRow::GetBorder() const
{
	return STableRow::GetBorder();
}

FReply SUnrealDiffDataTableListViewRow::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	STableRow::OnMouseButtonDown(MyGeometry, MouseEvent);
	
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UUnrealDiffAssetDelegate::OnDataTableRowSelected.Execute(bIsLocal, RowDataPtr->RowId);	
	}
	
	return FReply::Handled();
}

FReply SUnrealDiffDataTableListViewRow::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	STableRow::OnMouseButtonUp(MyGeometry, MouseEvent);

	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		UUnrealDiffAssetDelegate::OnDataTableRowSelected.Execute(bIsLocal, RowDataPtr->RowId);	

		TSharedRef<SWidget> MenuWidget = MakeRowActionsMenu();

		FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuWidget, MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect::ContextMenu);
	}
	
	return FReply::Handled();
}

TSharedRef<SWidget> SUnrealDiffDataTableListViewRow::MakeRowActionsMenu()
{
	FMenuBuilder MenuBuilder(true, NULL);
	MenuBuilder.AddMenuEntry(
		LOCTEXT("DataTableRowMenuActions_CopyName", "Copy Name"),
		LOCTEXT("DataTableRowMenuActions_CopyNamTooltip", "Copy row name"),
#if ENGINE_MAJOR_VERSION == 4
		FSlateIcon(FEditorStyle::GetStyleSetName(), "GenericCommands.Copy"),
#else
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "GenericCommands.Copy"),
#endif
		FUIAction(FExecuteAction::CreateRaw(this, &SUnrealDiffDataTableListViewRow::OnMenuActionCopyName))
	);
	
	MenuBuilder.AddMenuEntry(
		LOCTEXT("DataTableRowMenuActions_Copy", "Copy Value"),
		LOCTEXT("DataTableRowMenuActions_CopyTooltip", "Copy this row"),
#if ENGINE_MAJOR_VERSION == 4
		FSlateIcon(FEditorStyle::GetStyleSetName(), "GenericCommands.Copy"),
#else
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "GenericCommands.Copy"),
#endif
		FUIAction(FExecuteAction::CreateRaw(this, &SUnrealDiffDataTableListViewRow::OnMenuActionCopyValue))
	);
	
	// MenuBuilder.AddMenuEntry(
	// LOCTEXT("DataTableRowMenuActions_ShowDifference", "Show Difference"),
	// LOCTEXT("DataTableRowMenuActions_ShowDifferencTooltip", "Show Difference"),
	// 	FSlateIcon(FAppStyle::GetAppStyleSetName(), "SourceControl.Actions.Diff"),
	// 	FUIAction(FExecuteAction::CreateRaw(this, &SUnrealDiffDataTableListViewRow::OnMenuActionShowDifference))
	// );
	
	return MenuBuilder.MakeWidget();
}

void SUnrealDiffDataTableListViewRow::OnMenuActionCopyName()
{
	if (DataTableVisual)
	{
		DataTableVisual->CopyRowName(RowDataPtr->RowId);
	}
}

void SUnrealDiffDataTableListViewRow::OnMenuActionCopyValue()
{
	if (!RowDataPtr.IsValid())
	{
		return;
	}

	if (DataTableVisual)
	{
		if (!RowDataPtr->bIsRemoved)
		{
			DataTableVisual->CopyRow(bIsLocal, RowDataPtr->RowId);
		}
		else
		{
			DataTableVisual->CopyRow(bIsLocal, NAME_None);
		}
	}
}

void SUnrealDiffDataTableListViewRow::OnMenuActionShowDifference()
{
}

FReply SUnrealDiffDataTableListViewRow::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	STableRow::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
	return FReply::Handled();
}

FReply SUnrealDiffDataTableListViewRow::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	STableRow::OnKeyDown(MyGeometry, InKeyEvent);
	return FReply::Handled();
}

TSharedRef<SWidget> SUnrealDiffDataTableListViewRow::GenerateWidgetForColumn(const FName& InColumnId)
{
	if (InColumnId.IsEqual(TEXT("RowNumber")))
	{
		return SNew(SBox)
			.Padding(FMargin(4, 2, 4, 2))
			.IsEnabled(DataTableLayout->IsCellEnable(InColumnId, RowDataPtr->RowId))
			[
				SNew(STextBlock)
#if ENGINE_MAJOR_VERSION == 4
				.TextStyle(FEditorStyle::Get(), "DataTableEditor.CellText")
#else
				.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
#endif
				.Text(FText::FromString(FString::FromInt(RowDataPtr->RowNum)))
				.ColorAndOpacity(DataTableLayout->GetCellTextColor(InColumnId, RowDataPtr->RowId))
			];
	}
	
	if (InColumnId.IsEqual(TEXT("RowName")))
	{
		return SNew(SBox)
			.IsEnabled(DataTableLayout->IsCellEnable(InColumnId, RowDataPtr->RowId))
			.Padding(FMargin(4, 2, 4, 2))
			[
				SNew(SInlineEditableTextBlock)
				.IsReadOnly(true)
				.Text(FText::FromName(RowDataPtr->RowId))
				.ColorAndOpacity(DataTableLayout->GetCellTextColor(InColumnId, RowDataPtr->RowId))
			];
	}
	
	return MakeCellWidget(InColumnId);
}

TSharedRef<SWidget> SUnrealDiffDataTableListViewRow::MakeCellWidget(const FName& InColumnId)
{
	return SNew(SBox)
		.Padding(FMargin(4, 2, 4, 2))
		[
			SNew(STextBlock)
			.IsEnabled(DataTableLayout->IsCellEnable(InColumnId, RowDataPtr->RowId))
			.Margin(FMargin(5.f, 0.f, 0.f, 0.f))
#if ENGINE_MAJOR_VERSION == 4
			.TextStyle(FEditorStyle::Get(), "DataTableEditor.CellText")
#else
			.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
#endif
			.Text(DataTableLayout->GetCellText(InColumnId, RowDataPtr->RowId))
			.ColorAndOpacity(DataTableLayout->GetCellTextColor(InColumnId, RowDataPtr->RowId))
		];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
