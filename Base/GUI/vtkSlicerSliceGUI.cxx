#include "vtkObjectFactory.h"
#include "vtkCommand.h"
#include "vtkKWApplication.h"
#include "vtkKWWidget.h"
#include "vtkSlicerGUI.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSliceWidgetCollection.h"
#include "vtkSlicerSliceWidget.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkImageViewer2.h"
#include "vtkCornerAnnotation.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWScaleWithEntry.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkSlicerSliceGUI);
vtkCxxRevisionMacro(vtkSlicerSliceGUI, "$Revision: 1.0 $");


//---------------------------------------------------------------------------
vtkSlicerSliceGUI::vtkSlicerSliceGUI (  ) {

    this->Logic = NULL;
    this->SliceLogic = NULL;
    this->SliceWidgets = vtkSlicerSliceWidgetCollection::New();
}


//---------------------------------------------------------------------------
vtkSlicerSliceGUI::~vtkSlicerSliceGUI ( ) {

    if ( this->SliceWidgets ) {
        this->SliceWidgets->Delete();
    }
    this->Logic = NULL;
    this->SliceLogic = NULL;
}



//---------------------------------------------------------------------------
void vtkSlicerSliceGUI::AddSliceWidget ( vtkSlicerSliceWidget *w ){
    // Create if it doesn't exist already
    if ( this->SliceWidgets == NULL ) {
        this->SliceWidgets = vtkSlicerSliceWidgetCollection::New();
    }
    //Add widget
    this->SliceWidgets->AddItem ( w );
}


//---------------------------------------------------------------------------
void vtkSlicerSliceGUI::AddGUIObservers ( ) {
    

    // add observers ont SliceWidget(0)
    vtkSlicerSliceWidget *sw = this->GetSliceWidget (0);
    vtkImageViewer2 *iv = sw->GetImageViewer ();
    vtkKWScaleWithEntry *s = sw->GetScale () ;
    s->AddObserver (vtkCommand::ModifiedEvent, (vtkCommand *)this->GUICommand );

    // add observers onto SliceWidget(1) (do later)

    // add observers onto SliceWidget(2) (do later)
}


//---------------------------------------------------------------------------
void vtkSlicerSliceGUI::AddMrmlObservers ( ) {
    
    // add observers onto mrml scene
}



//---------------------------------------------------------------------------
void vtkSlicerSliceGUI::AddLogicObservers ( ) {

    // add observers onto mrmlscene.
    // this->Logic->GetMRMLScene()->AddObserver ( vtkCommand::ModifiedEvent,  (vtkCommand *)this->LogicCommand );
}



//---------------------------------------------------------------------------
void vtkSlicerSliceGUI::ProcessGUIEvents ( vtkObject *caller,
                                                   unsigned long event,
                                                   void *callData ) 
{

    vtkKWScaleWithEntry *scalewidget = vtkKWScaleWithEntry::SafeDownCast(caller);
    if (scalewidget == this->GetSliceWidget(0)->GetScale() && event == vtkCommand::ModifiedEvent )
        {
            vtkSlicerSliceWidget *sw = this->GetSliceWidget (0);
            vtkImageViewer2 *iv = sw->GetImageViewer ();
            vtkKWScaleWithEntry *s = sw->GetScale () ;

            // Set the current slice in the image viewer.
            if ( iv->GetSlice ( ) != s->GetValue ( ) ) {
                iv->SetSlice((int) s->GetValue ( ) );
            }
        }

}

//---------------------------------------------------------------------------
void vtkSlicerSliceGUI::ProcessMrmlEvents ( vtkObject *caller,
                                                   unsigned long event,
                                                   void *callData ) 
{
    // if a new volume is added to MrmlScene, then we'll have to update the
    // image viewer here.
}


//---------------------------------------------------------------------------
void vtkSlicerSliceGUI::ProcessLogicEvents ( vtkObject *caller,
                                                   unsigned long event,
                                                   void *callData ) 
{
    // process Logic changes
    // In this example, only thing that will come from the logic is new image data.
    // Steve is changing this to monitor the mrml scene directly rather tahn going
    // thru the logic.
    vtkMRMLScene *mrml = vtkMRMLScene::SafeDownCast(caller);
    
    if (mrml == (this->Logic->GetMRMLScene ( ) ) && event == vtkCommand::ModifiedEvent )
        {
            // If the MRML scene has changed, get the 0th volume node.
            // and set that as input into the ImageViewer.
            vtkMRMLScalarVolumeNode* volumenode = vtkMRMLScalarVolumeNode::SafeDownCast (this->Logic->GetMRMLScene()->GetNthNodeByClass( 0, "vtkMRMLScalarVolumeNode" ) );
            vtkSlicerSliceWidget *sw = this->GetSliceWidget (0);
            vtkImageViewer2 *iv = sw->GetImageViewer ();
            vtkKWRenderWidget *rw = sw->GetRenderWidget ();
            vtkKWScaleWithEntry *s = sw->GetScale () ;

            if ( volumenode && volumenode->GetImageData() != NULL )
                {
                    iv->SetInput ( volumenode->GetImageData( ) );
                    double *range = volumenode->GetImageData()->GetScalarRange ( );
                    iv->SetColorWindow ( range [1] - range [0] );
                    iv->SetColorLevel (0.5 * (range [1] - range [0] ));
                }
            else
                {
                    iv->SetInput (NULL);
                }
            iv->Render();


            // configure window, level, camera, etc.
            rw->ResetCamera ( );
            vtkCornerAnnotation *ca = rw->GetCornerAnnotation ( );
            ca->SetImageActor (iv->GetImageActor ( ) );

            // update range and value of slider.
            s->SetRange ( iv->GetSliceMin ( ), iv->GetSliceMax ( ) );
            s->SetValue (iv->GetSlice ( ) );
        }
}




// get slicewidget 0, 1, 2
//---------------------------------------------------------------------------
vtkSlicerSliceWidget* vtkSlicerSliceGUI::GetSliceWidget ( int SliceWidgetNum ) {

    return ( (vtkSlicerSliceWidget *) (this->SliceWidgets->GetItemAsObject( SliceWidgetNum ) ) );
}



// get slicewidget red, yellow, green
//---------------------------------------------------------------------------
vtkSlicerSliceWidget* vtkSlicerSliceGUI::GetSliceWidget ( char *SliceWidgetColor ) {
    if ( SliceWidgetColor == "r" || SliceWidgetColor == "R" )
        {
            return ( (vtkSlicerSliceWidget *) (this->SliceWidgets->GetItemAsObject( 0 )));
        } else if ( SliceWidgetColor == "g" || SliceWidgetColor == "G")
            {
                return ( (vtkSlicerSliceWidget *) (this->SliceWidgets->GetItemAsObject( 1 )));
            } else if ( SliceWidgetColor == "y" || SliceWidgetColor == "Y" )
                {
                    return ( (vtkSlicerSliceWidget *) (this->SliceWidgets->GetItemAsObject( 2 )));
                } else
                    {
                        return NULL;
                    }
}



//---------------------------------------------------------------------------
void vtkSlicerSliceGUI::BuildGUI (  ) {

    vtkSlicerGUI *app = (vtkSlicerGUI *)this->GetApplication();
    

    vtkSlicerSliceWidget *MainSlice0 = vtkSlicerSliceWidget::New ( );
    MainSlice0->SetApplication (app);
    MainSlice0->Create ( );
    this->AddSliceWidget ( MainSlice0 );

    // create these others for now, but they are not observed.
    vtkSlicerSliceWidget *MainSlice1 = vtkSlicerSliceWidget::New ( );
    MainSlice1->SetApplication (app);
    MainSlice1->Create ( );
    this->AddSliceWidget ( MainSlice1 );

    vtkSlicerSliceWidget *MainSlice2 = vtkSlicerSliceWidget::New ( );
    MainSlice2->SetApplication (app);
    MainSlice2->Create ( );
    this->AddSliceWidget ( MainSlice2 );
}





